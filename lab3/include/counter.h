#pragma once

#include "logger.h"
#include <ctime>
#include <mutex>
#include <string>

namespace moski {

/**
 * @class Counter
 * @brief Thread-safe counter class for managing and logging counter values.
 */
class Counter {
public:
  /**
   * @brief Constructs a Counter object.
   * @param `initial_value` Initial value of the counter (default is 0).
   */
  explicit Counter(int initial_value = 0,
                   std::string const &log_path = "main.log");

  /**
   * @brief Gets the current value of the counter.
   * @return The current counter value.
   */
  int get_value() const;

  /**
   * @brief Sets the counter to a new value.
   * @param `n` The value to set the counter to.
   */
  void set_value(int const &n);

  /**
   * @brief Sets the value to `n`.
   * @param `n` The new value to set to.
   */
  void operator=(int const &n);

  /**
   * @brief Increments the value by `n`.
   * @param `n` The number to add.
   */
  void operator+=(int const &n);

  /**
   * @brief Decreases the value by `n`.
   * @param `n` The number to substract.
   */
  void operator-=(int const &n);

  /**
   * @brief Increments the value `n` times.
   * @param `n` The number to multiply by.
   */
  void operator*=(int const &n);

  /**
   * @brief Decreases the value `n` times.
   * @param `n` The number to divide by.
   */
  void operator/=(int const &n);

  /**
   * @brief Writes a message to the log file with a timestamp and the current
   * process' pid.
   * @param `message` The message to log.
   */
  void log(std::string const &message) const;

private:
  int value_; ///< Current value of the counter.
  mutable std::mutex
      mutex_; ///< Mutex to ensure thread-safe access to the counter.
  mutable Logger logger_; ///< Logger object
};

} // namespace moski
