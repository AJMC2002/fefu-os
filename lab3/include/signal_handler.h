#include "process/process.h"

namespace moski {

/**
 * @brief Singleton class for managing signal handling and cleanup.
 */
class SignalHandler {
  public:
    /**
     * @brief Returns the singleton instance of SignalHandler.
     * @return Reference to the singleton instance.
     */
    static SignalHandler &get_instance();

    /**
     * @brief Registers the signal handler for SIGINT and SIGTERM.
     */
    void register_signals();

    /**
     * @brief Sets the current Process object for cleanup.
     * @param process Pointer to the Process object.
     */
    void set_process(Process *process);

    // Delete copy constructor and assignment operator to enforce singleton
    SignalHandler(const SignalHandler &) = delete;
    SignalHandler &operator=(const SignalHandler &) = delete;

  private:
    /**
     * @brief Private constructor to enforce singleton pattern.
     */
    SignalHandler() = default;

    /**
     * @brief Handles signals (e.g., SIGINT, SIGTERM) and performs cleanup.
     * @param signal The signal number (e.g., SIGINT for Ctrl+C).
     */
    static void handle_signal(int signal);

    /// Pointer to the current Process object for cleanup.
    Process *process_ = nullptr;
};

} // namespace moski
