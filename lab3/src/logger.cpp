#include "logger.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unistd.h>

namespace moski {

Logger::Logger() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!std::filesystem::exists(dir_)) {
        std::filesystem::create_directories(dir_);
    }

    std::string path = dir_ + "/" + get_current_time() + ".log";
    file_ = std::ofstream(path, std::ios::out | std::ios::app);
    if (!file_.is_open())
        throw std::ios_base::failure("Failed to open log file: " + path);
    else
        std::cout << "Opened log file at: " + path;
}

Logger::~Logger() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (file_.is_open()) {
        file_.close();
    }
}

void Logger::log(const std::string &message) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!file_.is_open()) {
        throw std::ios_base::failure("Log file is not open.");
    }

    file_ << "[" << get_current_time() << "]["
          << std::to_string(get_current_pid()) << "] " << message << "\n";
}

Logger::Logger(Logger &&other) noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    file_ = std::move(other.file_);
}

Logger &Logger::operator=(Logger &&other) noexcept {
    if (this != &other) {
        std::lock_guard<std::mutex> lock(mutex_);
        file_ = std::move(other.file_);
    }
    return *this;
}

Logger::pid_type Logger::get_current_pid() const {
#if defined(__unix)
    return getpid();
#elif defined(_WIN32)
    return GetCurrentProcessId();
#endif
};

std::string Logger::get_current_time() const {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now.time_since_epoch()) %
                        1000;

    std::stringstream time_stream;
    time_stream << std::put_time(std::localtime(&time_t_now),
                                 "%Y-%m-%d %H:%M:%S")
                << "." << std::setfill('0') << std::setw(3)
                << milliseconds.count();
    return time_stream.str();
}

} // namespace moski
