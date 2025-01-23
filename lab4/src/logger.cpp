#include "logger.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace moski {

Logger::Logger(const std::string& log_dir,
			   const std::chrono::hours& max_duration)
	: log_dir_(log_dir), max_duration_(max_duration) {
	std::lock_guard<std::mutex> lock(mutex_);

	if (!std::filesystem::exists(log_dir_)) {
		std::filesystem::create_directories(log_dir_);
	}

	std::string log_path = log_dir_ + "/" + get_current_timestamp() + ".log";
	file_.open(log_path, std::ios::out | std::ios::app);
	if (!file_.is_open()) {
		throw std::ios_base::failure("Failed to open log file: " + log_path);
	}

	std::cout << "Opened log file at: \"" << log_path << "\"\n";
}

void Logger::log(const std::string& message) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (!file_.is_open()) {
		throw std::ios_base::failure("Log file is not open.");
	}

	file_ << "[" << get_current_timestamp() << "] " << message << "\n";
	file_.flush();
}

void Logger::cleanup() {
	std::lock_guard<std::mutex> lock(mutex_);
	delete_old_entries();
}

Logger::~Logger() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (file_.is_open()) {
		file_.close();
	}
}

std::string Logger::get_current_timestamp() const {
	auto now = std::chrono::system_clock::now();
	auto now_time_t = std::chrono::system_clock::to_time_t(now);
	auto now_tm = *std::localtime(&now_time_t);

	std::ostringstream oss;
	oss << std::put_time(&now_tm, "%Y-%m-%d_%H-%M-%S");
	return oss.str();
}

void Logger::delete_old_entries() {
	auto now = std::chrono::system_clock::now();

	for (const auto& entry : std::filesystem::directory_iterator(log_dir_)) {
		if (entry.is_regular_file()) {
			auto file_time = std::filesystem::last_write_time(entry);

			auto file_time_system =
				std::chrono::system_clock::now() -
				(std::filesystem::file_time_type::clock::now() - file_time);

			auto file_age = now - file_time_system;

			if (file_age > max_duration_) {
				std::filesystem::remove(entry);
				std::cout << "Deleted old log file: " << entry.path() << "\n";
			}
		}
	}
}

}  // namespace moski