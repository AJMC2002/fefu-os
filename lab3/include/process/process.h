#pragma once

#include "counter.h"
#include "utils.h"

#include <atomic>
#include <cstddef>

#if defined(__unix__)
#include <semaphore.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace moski {

/**
 * @brief Represents the layout of shared memory used by processes.
 */
struct SharedMemoryLayout {
    sem_t semaphore;           ///< Semaphore for synchronization.
    Counter counter;           ///< Thread-safe counter for shared operations.
    pid_type leader_pid;       ///< Process ID in charge of spawning.
    std::size_t process_count; ///< Count of processes currently active.
};

/**
 * @brief Manages a process that interacts with shared memory.
 */
class Process {
  public:
    /**
     * @brief Constructs a Process object.
     * @param shm_name Name of the shared memory segment.
     * @param shm_size Size of the shared memory segment.
     */
    Process(const char *shm_name, std::size_t shm_size);

    /**
     * @brief Runs the process, managing shared memory and spawning threads.
     */
    void run();

    /**
     * @brief Destructor for the Process object.
     * @details Cleans up shared memory and performs necessary cleanup
     * operations.
     */
    ~Process();

  private:
    const char *shm_name_;    ///< Name of the shared memory segment.
    std::size_t shm_size_;    ///< Size of the shared memory segment.
    SharedMemoryLayout *shm_; ///< Pointer to the shared memory layout.
    pid_type pid_;            ///< Process ID of the current process.
    bool is_leader_; ///< Flag indicating if this process is the leader.

    std::atomic<bool>
        is_running_; ///< Flag to control the running state of the process.
    std::atomic<bool> can_spawn_; ///< Flag to control child process spawning.
    std::atomic<int> active_children_; ///< Number of active child processes.

    /*
     * @bried Spawns both children processes.
     */
    void spawn_child_processes();
};

} // namespace moski
