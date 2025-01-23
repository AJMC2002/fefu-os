#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>

namespace moski {

class Logger {
   public:
	/**
	 * @brief Constructs a Logger instance.
	 * @param log_dir Directory to store log files.
	 * @param max_duration Maximum duration to keep logs (e.g., 24 hours, 1
	 * month).
	 */
	explicit Logger(const std::string& log_dir,
					const std::chrono::hours& max_duration);

	/**
	 * @brief Writes a message to the log file with a timestamp.
	 * @param message The message to log.
	 */
	void log(const std::string& message);

	/**
	 * @brief Cleans up old log entries based on the max_duration.
	 */
	void cleanup();

	/**
	 * @brief Destructor for Logger. Closes the log file if open.
	 */
	~Logger();

   private:
	std::ofstream file_;			   ///< Stream for writing to the log file.
	mutable std::mutex mutex_;		   ///< Mutex for thread-safe logging.
	std::string log_dir_;			   ///< Directory for all the logs.
	std::chrono::hours max_duration_;  ///< Maximum duration to keep logs.

	/**
	 * @brief Gets the current timestamp as a string.
	 * @return Timestamp in "YYYY-MM-DD_HH-MM-SS" format.
	 */
	std::string get_current_timestamp() const;

	/**
	 * @brief Deletes old log entries based on the max_duration.
	 */
	void delete_old_entries();
};

}  // namespace moski