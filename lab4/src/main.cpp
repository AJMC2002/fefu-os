#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "logger.h"
#include "serial_port.h"
#include "temperature_reader.h"

namespace {

const std::string temp_log_dir = "./logs/temperature";
const std::string hourly_avg_log_dir = "./logs/hourly_average";
const std::string daily_avg_log_dir = "./logs/daily_average";

moski::Logger temp_logger(temp_log_dir, std::chrono::hours(24));
moski::Logger hourly_avg_logger(hourly_avg_log_dir,
								std::chrono::hours(24 * 30));
moski::Logger daily_avg_logger(daily_avg_log_dir, std::chrono::hours(24 * 365));

double calculate_average(const std::vector<double>& temperatures) {
	double sum = 0.0;
	for (const auto& temp : temperatures) {
		sum += temp;
	}
	return sum / temperatures.size();
}

void run_temperature_monitor() {
	std::vector<double> hourly_temperatures;
	std::vector<double> daily_averages;

	auto start_time = std::chrono::steady_clock::now();

	while (true) {
		double temperature = read_temperature_from_serial();

		// Log the raw temperature
		temp_logger.log("Temperature: " + std::to_string(temperature) + "°C");

		// Add the temperature to the hourly and daily buffers
		hourly_temperatures.push_back(temperature);

		// Check if an hour has passed
		auto current_time = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::hours>(current_time -
														   start_time)
				.count() >= 1) {
			double hourly_avg = calculate_average(hourly_temperatures);
			hourly_avg_logger.log(
				"Hourly Average: " + std::to_string(hourly_avg) + "°C");

			daily_averages.push_back(hourly_avg);

			hourly_temperatures.clear();
			start_time = current_time;

			if (daily_averages.size() == 24) {
				double daily_avg = calculate_average(daily_averages);
				daily_avg_logger.log(
					"Daily Average: " + std::to_string(daily_avg) + "°C");

				daily_averages.clear();
			}
		}

		temp_logger.cleanup();
		hourly_avg_logger.cleanup();
		daily_avg_logger.cleanup();

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

}  // namespace

int main() {
	try {
		run_temperature_monitor();
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}