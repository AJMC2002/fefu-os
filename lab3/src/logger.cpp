#include "logger.h"
#include "utils.h"

#include <filesystem>
#include <fstream>
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
        std::cout << "Opened log file at: " + path + "\n";
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

    file_ << "[" << get_current_time() << "][" << std::to_string(get_current_pid())
          << "] " << message << "\n";
}

} // namespace moski
