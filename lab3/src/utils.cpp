#include "utils.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace moski {

pid_type get_current_pid() {
#if defined(__unix)
    return getpid();
#elif defined(_WIN32)
    return GetCurrentProcessId();
#endif
}

std::string get_current_time() {
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
