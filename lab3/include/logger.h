#pragma once

#include <fstream>
#include <mutex>

#if defined(__unix__)
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace moski {

class Logger {
#if defined(__unix__)
    /// Alias for the pid type based on the OS
    using pid_type = pid_t;
#elif defined(_WIN32)
    /// Alias for the pid type based on the OS
    using pid_type = DWORD;
#endif

  public:
    /**
     * @brief Constructs a Logger instance.
     */
    explicit Logger();

    /**
     * @brief Writes a message to the log file with a timestamp and the current
     * process' pid.
     * @param `message` The message to log.
     */
    void log(const std::string &message);

    /**
     * @brief Flushes the internal state of the Logger (e.g., logs) to ensure
     * all data is written and closes the file.
     */
    void cleanup();

    /**
     * @brief Destructor for Logger. Closes the log file if open.
     */
    ~Logger();

  private:
    std::ofstream file_;         ///< Stream for writing to the log file.
    mutable std::mutex mutex_;   ///< Mutex for thread-safe logging.
    std::string dir_ = "./logs"; ///< Directory for all the logs.
};

} // namespace moski
