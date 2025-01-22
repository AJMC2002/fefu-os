#pragma once

#include <csignal>
#include <string>

#if defined(__unix__)
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace moski {

#if defined(__unix__)
/// Alias for the pid type based on the OS
using pid_type = pid_t;
#elif defined(_WIN32)
/// Alias for the pid type based on the OS
using pid_type = DWORD;
#endif

/**
 * @brief Helper function to get the current pid.
 * @return A process id number. Can be either a `pid_t` or a `DWORD`.
 */
pid_type get_current_pid();

/**
 * @brief Helper function to get the current time in a formatted string.
 * @return A string in the format `YYYY-MM-DD HH:MM:SS.mmm`.
 */
std::string get_current_time();

} // namespace moski
