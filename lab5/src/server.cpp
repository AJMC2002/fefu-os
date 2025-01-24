#include "server.h"

#include <iostream>

Server::Server(Database& db, LogParser& temp_parser, LogParser& hourly_parser,
			   LogParser& daily_parser)
	: db_(db),
	  temp_parser_(temp_parser),
	  hourly_parser_(hourly_parser),
	  daily_parser_(daily_parser) {}

void Server::start(int port) {
	setup_routes();
	std::cout << "Starting HTTP server on port " << port << "..." << std::endl;
	server_.listen("0.0.0.0", port);
}

void Server::setup_routes() {
	// Endpoint to get the latest temperature
	server_.Get("/temperature", [this](const httplib::Request& req,
									   httplib::Response& res) {
		try {
			pqxx::work txn(db_.connection());
			auto result = txn.exec1(
				"SELECT temperature FROM temperature_data ORDER BY timestamp "
				"DESC LIMIT 1");
			if (result.empty()) {
				res.status = 404;
				res.set_content(R"({"error": "No temperature data found"})",
								"application/json");
				return;
			}
			txn.commit();

			double temperature = result[0].as<double>();
			res.set_content(
				R"({"temperature": )" + std::to_string(temperature) + "}",
				"application/json");
		} catch (const std::exception& e) {
			res.status = 500;
			res.set_content(R"({"error": ")" + std::string(e.what()) + "\"}",
							"application/json");
		}
	});

	// Endpoint to get statistics (hourly or daily averages)
	server_.Get("/statistics", [this](const httplib::Request& req,
									  httplib::Response& res) {
		try {
			std::string period = req.get_param_value("period");
			if (period != "hourly" && period != "daily") {
				res.status = 400;
				res.set_content(
					R"({"error": "Invalid period. Use 'hourly' or 'daily'."})",
					"application/json");
				return;
			}

			pqxx::work txn(db_.connection());
			std::string query;
			if (period == "hourly") {
				query =
					"SELECT hour, average_temperature FROM hourly_averages "
					"ORDER BY hour DESC";
			} else {
				query =
					"SELECT day, average_temperature FROM daily_averages ORDER "
					"BY day DESC";
			}

			auto result = txn.exec(query);
			txn.commit();

			std::string json_response = "[";
			for (const auto& row : result) {
				json_response += R"({"timestamp": ")" +
								 row[0].as<std::string>() + "\", " +
								 R"("average_temperature": )" +
								 std::to_string(row[1].as<double>()) + "},";
			}
			if (!result.empty()) {
				json_response.pop_back();  // Remove the trailing comma
			}
			json_response += "]";

			res.set_content(json_response, "application/json");
		} catch (const std::exception& e) {
			res.status = 500;
			res.set_content(R"({"error": ")" + std::string(e.what()) + "\"}",
							"application/json");
		}
	});
}