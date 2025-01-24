#include <iostream>

#include "database.h"
#include "log_parser.h"
#include "server.h"

int main() {
	try {
		// Initialize the database connection
		Database db(
			"dbname=temperature_db user=le-moski password=Adrianjmc2002");

		// Parse temperature logs
		LogParser temp_parser(db, LogParser::LogType::Temperature);
		temp_parser.parse_logs("../lab4/logs/temperature");

		// Parse hourly average logs
		LogParser hourly_parser(db, LogParser::LogType::HourlyAverage);
		hourly_parser.parse_logs("../lab4/logs/hourly_average");

		// Parse daily average logs
		LogParser daily_parser(db, LogParser::LogType::DailyAverage);
		daily_parser.parse_logs("../lab4/logs/daily_average");

		// Start the HTTP server
		Server server(db, temp_parser, hourly_parser, daily_parser);
		server.start(8080);
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}