#include "log_parser.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {
std::string convert_to_sql_timestamp(const std::string& log_timestamp) {
	std::string sql_timestamp = log_timestamp;

	// Replace the first '_' with a space
	size_t underscore_pos = sql_timestamp.find('_');
	if (underscore_pos != std::string::npos) {
		sql_timestamp[underscore_pos] = ' ';
	}

	// Replace the '-' between hours, minutes, and seconds with ':'
	for (size_t i = underscore_pos + 1; i < sql_timestamp.length(); ++i) {
		if (sql_timestamp[i] == '-') {
			sql_timestamp[i] = ':';
		}
	}

	return sql_timestamp;
}
}  // namespace

// Correct constructor implementation
LogParser::LogParser(Database& db, LogType log_type)
	: db_(db), log_type_(log_type) {}

void LogParser::parse_logs(const std::string& log_dir) {
	for (const auto& entry : std::filesystem::directory_iterator(log_dir)) {
		if (entry.is_regular_file()) {
			parse_file(entry.path());
		}
	}
	update_averages();
}

void LogParser::parse_file(const std::string& file_path) {
	std::ifstream file(file_path);
	std::string line;
	while (std::getline(file, line)) {
		size_t timestamp_start = line.find('[');
		size_t timestamp_end = line.find(']');
		if (timestamp_start == std::string::npos ||
			timestamp_end == std::string::npos) {
			continue;  // Skip malformed lines
		}

		std::string log_timestamp = line.substr(
			timestamp_start + 1, timestamp_end - timestamp_start - 1);
		std::string sql_timestamp =
			convert_to_sql_timestamp(log_timestamp); 
		std::string value_str;

		switch (log_type_) {
			case LogType::Temperature:
				value_str =
					line.substr(line.find("Temperature: ") +
								12);  // "Temperature: " is 12 characters
				break;
			case LogType::HourlyAverage:
				value_str =
					line.substr(line.find("Hourly Average: ") +
								15);  // "Hourly Average: " is 15 characters
				break;
			case LogType::DailyAverage:
				value_str =
					line.substr(line.find("Daily Average: ") +
								14);  // "Daily Average: " is 14 characters
				break;
			default:
				continue;  // Unknown log type
		}

		double value = std::stod(value_str);
		raw_data_.emplace_back(sql_timestamp,
							   value);	// Use the converted timestamp

		// Insert into the appropriate table
		switch (log_type_) {
			case LogType::Temperature:
				db_.insert_temperature(sql_timestamp, value);
				break;
			case LogType::HourlyAverage:
				db_.upsert_hourly_average(sql_timestamp, value);
				break;
			case LogType::DailyAverage:
				db_.upsert_daily_average(sql_timestamp, value);
				break;
			default:
				break;
		}
	}
}

void LogParser::update_averages() {
	if (log_type_ != LogType::Temperature) {
		return;	 // Skip for hourly and daily averages
	}

	for (const auto& [timestamp, _] : raw_data_) {
		std::string hour =
			timestamp.substr(11, 2);  // Extract hour (YYYY-MM-DD HH)
		std::string day = timestamp.substr(8, 2);	// Extract day (YYYY-MM-DD)

		// Check if hourly average already exists
		pqxx::work txn(db_.connection());
		auto hourly_result = txn.exec(
			"SELECT 1 FROM hourly_averages WHERE hour = " + txn.quote(hour));
		if (hourly_result.empty()) {
			double hourly_avg = db_.calculate_hourly_average(hour);
			db_.upsert_hourly_average(hour, hourly_avg);
		}

		// Check if daily average already exists
		auto daily_result = txn.exec(
			"SELECT 1 FROM daily_averages WHERE day = " + txn.quote(day));
		if (daily_result.empty()) {
			double daily_avg = db_.calculate_daily_average(day);
			db_.upsert_daily_average(day, daily_avg);
		}

		txn.commit();
	}
}
