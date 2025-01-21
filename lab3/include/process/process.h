#pragma once

#include "counter.h"
#include <atomic>
#include <cstddef>
#include <deque>
#include <mutex>

namespace moski {

struct SharedMemoryLayout {
#if defined(__unix__)
    /// Alias for the pid type based on the OS
    using pid_type = pid_t;
#elif defined(_WIN32)
    /// Alias for the pid type based on the OS
    using pid_type = DWORD;
#endif

    std::mutex mutex;
    Counter counter;
    std::deque<pid_type> pids;
};

class Process {
  public:
    Process(const char *shm_name, std::size_t shm_size,
            SharedMemoryLayout *shm);

    void run();

    ~Process();

  private:
    const char *shm_name_;
    std::size_t shm_size_;
    SharedMemoryLayout *shm_;
    bool is_leader_;
    std::atomic<bool> is_running_;
    std::atomic<bool> can_spawn_;
    std::atomic<int> active_children_;

    void timer(Counter &counter);
    void spawner(Counter &counter);
};

} // namespace moski
