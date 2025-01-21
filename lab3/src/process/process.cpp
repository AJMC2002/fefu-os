#include "process/process.h"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <mutex>
#include <sys/mman.h>

namespace moski {

Process::Process(const char *shm_name, std::size_t shm_size,
                 SharedMemoryLayout *shm)
    : shm_name_(shm_name), shm_size_(shm_size), shm_(shm), is_leader_(false),
      is_running_(true), can_spawn_(false), active_children_(0) {
    std::lock_guard<std::mutex> lock(shm->mutex);
    if (shm->pids.empty()) {
        is_leader_ = true;
        shm->counter.log("This process is the leader.");
    } else {
        shm->counter.log("This process is a follower.");
    }
    shm->pids.emplace_back(getpid());
}

void Process::run() {
    std::cout << "KINDA WORKING LOLS\n";

    sleep(2);
}

Process::~Process() {
    std::lock_guard<std::mutex> lock(shm_->mutex);

    std::cout << "Exiting...\n";

    if (is_leader_) {
        is_leader_ = false;

        SharedMemoryLayout::pid_type pid =
#if defined(__unix)
            getpid();
#elif defined(_WIN32)
            GetCurrentProcessId();
#endif

        auto it = std::find(shm_->pids.begin(), shm_->pids.end(), pid);
        if (it != shm_->pids.end())
            shm_->pids.erase(it);

        if (!shm_->pids.empty()) {
            SharedMemoryLayout::pid_type new_leader = shm_->pids.front();
            shm_->counter.log("Leadership transferred to PID: " +
                              std::to_string(new_leader));
        } else {
            shm_->counter.log("No processes left. Unlinking shared memory.");

            if (shm_unlink(shm_name_) == -1) {
                std::cerr << "Failed to unlink shared memory: "
                          << strerror(errno) << "\n";
            } else {
                shm_->counter.log("Shared memory unlinked successfully.");
            }
        }
    }

    if (munmap(shm_, shm_size_) == -1) {
        std::cerr << "Failed to unmap shared memory: " << strerror(errno)
                  << "\n";
    } else {
        std::cout << "Shared memory unmapped successfully. Finished work.\n";
    }
}

} // namespace moski
