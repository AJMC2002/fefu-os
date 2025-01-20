#pragma once

#include <fstream>
#include <mutex>
#include <string>
#if defined(__unix)
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace moski {

class Logger {
public:
  friend class Counter;

  /**
   * @brief Constructs a Logger instance.
   * @param `path` Path to the log file.
   */
  explicit Logger(const std::string &path);

  /**
   * @brief Destructor for Logger. Closes the log file if open.
   */
  ~Logger();

  /**
   * @brief Writes a message to the log file with a timestamp and the current
   * process' pid.
   * @param `message` The message to log.
   */
  void log(const std::string &message);

  /**
   * @brief Prevent copying of Logger instances.
   */
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  /**
   * @brief Allow moving of Logger instances.
   */
  Logger(Logger &&other) noexcept;
  Logger &operator=(Logger &&other) noexcept;

private:
  std::ofstream file_;       ///< Stream for writing to the log file.
  mutable std::mutex mutex_; ///< Mutex for thread-safe logging.

  /**
   * @brief Helper function to get the current time in a formatted string.
   * @return A string in the format `YYYY-MM-DD HH:MM:SS.mmm`.
   */
  std::string get_current_time() const;

  /**
   * @brief Helper function to get the current process' PID.
   * @return A DW
   */
#if defined(__unix)
  __pid_t
#elif defined(_WIN32)
  DWORD
#endif
  get_current_pid() const;
};

} // namespace moski
