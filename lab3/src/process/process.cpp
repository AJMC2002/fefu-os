#include "process/process.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

namespace moski {

namespace {

SharedMemoryLayout::pid_type get_pid() {
#if defined(__unix)
    return getpid();
#elif defined(_WIN32)
    return GetCurrentProcessId();
#endif
}

} // namespace

Process::Process(const char *shm_name, std::size_t shm_size)
    : shm_name_(shm_name), shm_size_(shm_size), pid_(get_pid()),
      is_leader_(false), is_running_(true), can_spawn_(false),
      active_children_(0) {
    std::cout << "Initiating shared memory...\n";
    int shm_fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shm_fd == -1 && errno == EEXIST)
        shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    else
        is_leader_ = true;

    if (shm_fd == -1) {
        std::stringstream ss;
        ss << "Failed to open or create shared memory: " << strerror(errno)
           << "\n";
        throw std::runtime_error(ss.str());
    }

    if (ftruncate(shm_fd, shm_size) == -1) {
        std::stringstream ss;
        ss << "Failed to resize shared memory: " << strerror(errno) << "\n";
        shm_unlink(shm_name);
        throw std::runtime_error(ss.str());
    }

    void *shm_ptr =
        mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        std::stringstream ss;
        ss << "Failed to map shared memory: " << strerror(errno) << "\n";
        shm_unlink(shm_name);
        throw std::runtime_error(ss.str());
    }

    close(shm_fd);

    std::cout << "Shared memory successfully initiated.\n";

    shm_ = static_cast<moski::SharedMemoryLayout *>(shm_ptr);

    if (is_leader_) {
        shm_ = new SharedMemoryLayout();
        shm_->counter.log("This process is the leader.");
    } else {
        shm_->counter.log("This process is a follower.");
    }
    shm_->pids.emplace_back(pid_);
}

void Process::run() {
    std::thread leader_thread([this]() {
        while (is_running_) {
            if (!shm_->pids.empty() && shm_->pids.front() == pid_) {
                is_leader_ = true;
            } else {
                is_leader_ = false;
            }

            if (is_leader_) {
                std::cout << "This process is the leader.\n";
            } else {
                std::cout << "This process is a follower.\n";
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });

    std::thread stop_thread([this]() {
        std::cout << "Sleeping for 10 seconds...\n";
        sleep(4);

        is_running_ = false;
        std::cout << "Stopping the process...\n";
    });

    leader_thread.join();
    stop_thread.join();
}

Process::~Process() {
    std::cout << "Exiting...\n";

    auto it = std::find(shm_->pids.begin(), shm_->pids.end(), pid_);
    if (it != shm_->pids.end())
        shm_->pids.erase(it);

    bool unlink = false;
    if (!shm_->pids.empty()) {
        shm_->counter.log("Process " + std::to_string(pid_) + " exiting.");
        if (is_leader_) {
            SharedMemoryLayout::pid_type new_leader = shm_->pids.front();
            shm_->counter.log("Leadership transferred to PID: " +
                              std::to_string(new_leader));
        }
    } else {
        unlink = true;
        if (is_leader_)
            shm_->counter.log("No processes left. Unlinking shared memory.");
    }

    if (munmap(static_cast<void *>(shm_), shm_size_) == -1) {
        std::cerr << "Failed to unmap shared memory: " << strerror(errno)
                  << "\n";
    } else {
        std::cout << "Shared memory unmapped successfully. Finished work.\n";
    }

    if (unlink) {
        if (shm_unlink(shm_name_) == -1) {
            std::cerr << "Failed to unlink shared memory: " << strerror(errno)
                      << "\n";
        }
    }
}

} // namespace moski
