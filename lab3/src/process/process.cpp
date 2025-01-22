#include "process/process.h"
#include "utils.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__)
#include <fcntl.h>
#include <sys/mman.h>
#endif

namespace moski {

Process::Process(const char *shm_name, std::size_t shm_size)
    : shm_name_(shm_name), shm_size_(shm_size), pid_(get_current_pid()),
      is_leader_(false), is_running_(true), can_spawn_(false),
      active_children_(0) {
    std::cout << "Initiating shared memory...\n";
    int shm_fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shm_fd == -1 && errno == EEXIST)
        shm_fd = shm_open(shm_name, O_RDWR, 0666);
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
        new (shm_) SharedMemoryLayout();
        sem_init(&shm_->semaphore, 1, 1);
    }

    sem_wait(&shm_->semaphore);
    if (is_leader_)
        shm_->counter.log("This process is the leader.");
    else
        shm_->counter.log("This process is a follower.");
    shm_->pids.emplace_back(pid_);
    sem_post(&shm_->semaphore);
}

void Process::run() {
    std::thread counter_thread([this]() {
        while (is_running_) {
            sem_wait(&shm_->semaphore);
            shm_->counter += 1;
            sem_post(&shm_->semaphore);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    });

    std::thread leader_thread([this]() {
        while (is_running_) {
            sem_wait(&shm_->semaphore);
            if (!shm_->pids.empty() && shm_->pids.front() == pid_) {
                is_leader_ = true;
            } else {
                is_leader_ = false;
            }
            sem_post(&shm_->semaphore);

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
        std::this_thread::sleep_for(std::chrono::seconds(4));

        is_running_ = false;
        std::cout << "Stopping the process...\n";
    });

    counter_thread.join();
    leader_thread.join();
    stop_thread.join();
}

Process::~Process() {
    std::cout << "Exiting...\n";

    sem_wait(&shm_->semaphore);
    auto it = std::find(shm_->pids.begin(), shm_->pids.end(), pid_);
    if (it != shm_->pids.end())
        shm_->pids.erase(it);

    bool unlink = false;
    if (!shm_->pids.empty()) {
        shm_->counter.log("Process " + std::to_string(pid_) + " exiting.");
        if (is_leader_) {
            pid_type new_leader = shm_->pids.front();
            shm_->counter.log("Leadership transferred to PID: " +
                              std::to_string(new_leader));
        }
    } else {
        unlink = true;
        if (is_leader_)
            shm_->counter.log("No processes left. Unlinking shared memory.");
    }
    sem_post(&shm_->semaphore);

    if (unlink) {
        shm_->counter.~Counter();
    }

    if (unlink) {
        sem_destroy(&shm_->semaphore);
    }

    if (munmap(shm_, shm_size_) == -1) {
        std::cerr << "Failed to unmap shared memory: " << strerror(errno)
                  << "\n";
        shm_unlink(shm_name_);
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
