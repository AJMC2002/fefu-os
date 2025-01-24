#pragma once

#include <string>
#include <vector>

#include "database.h"

class LogParser {
   public:
	enum class LogType { Temperature, HourlyAverage, DailyAverage };

	LogParser(Database& db, LogType log_type);

	void parse_logs(const std::string& log_dir);

   private:
	void parse_file(const std::string& file_path);
	void update_averages();

	Database& db_;
	LogType log_type_;
	std::vector<std::pair<std::string, double>> raw_data_;
};