#include "process/process.h"
#include "process/subprocessA.h"
#include "utils.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
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
        if (is_leader_)
            shm_unlink(shm_name);
        throw std::runtime_error(ss.str());
    }

    void *shm_ptr =
        mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        std::stringstream ss;
        ss << "Failed to map shared memory: " << strerror(errno) << "\n";
        if (is_leader_)
            shm_unlink(shm_name);
        throw std::runtime_error(ss.str());
    }

    close(shm_fd);

    shm_ = static_cast<moski::SharedMemoryLayout *>(shm_ptr);

    if (!is_leader_) {
        if (kill(shm_->leader_pid, 0) != 0)
            is_leader_ = true;
    }

    if (is_leader_) {
        new (shm_) SharedMemoryLayout();
        sem_init(&shm_->semaphore, 1, 1);
    }

    sem_wait(&shm_->semaphore);
    if (is_leader_) {
        shm_->leader_pid = pid_;
        shm_->process_count = 0;
        shm_->counter.log("This process is the leader.");
    } else
        shm_->counter.log("This process is a follower.");
    shm_->process_count++;
    sem_post(&shm_->semaphore);

    std::cout << "Shared memory successfully initiated.\n";
}

void Process::run() {
    std::thread leader_thread([this]() {
        while (is_running_) {
            sem_wait(&shm_->semaphore);
            if (shm_->leader_pid == pid_ || shm_->leader_pid == -1) {
                shm_->leader_pid = pid_;
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

            std::this_thread::sleep_for(std::chrono::milliseconds(350));
        }
    });

    std::thread counter_thread([this]() {
        while (is_running_) {
            sem_wait(&shm_->semaphore);
            shm_->counter += 1;
            sem_post(&shm_->semaphore);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    });

    std::thread logger_thread([this]() {
        while (is_running_) {
            sem_wait(&shm_->semaphore);
            shm_->counter.log("Current value: " +
                              std::to_string(shm_->counter.get_value()));
            sem_post(&shm_->semaphore);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::thread spawner_thread([this]() {
        while (is_running_) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            if (is_leader_) {
                spawn_child_processes();
            }
        }
    });

    leader_thread.join();
    counter_thread.join();
    logger_thread.join();
    spawner_thread.join();
}

Process::~Process() {
    std::cout << "Exiting...\n";

    sem_wait(&shm_->semaphore);
    shm_->counter.log("Process " + std::to_string(pid_) + " exiting.");
    if (is_leader_)
        shm_->leader_pid = -1;
    shm_->process_count--;
    bool have_to_unlink = shm_->process_count <= 0;
    sem_post(&shm_->semaphore);

    if (have_to_unlink) {
        shm_->counter.cleanup();
        sem_destroy(&shm_->semaphore);
    }

    if (munmap(shm_, shm_size_) == -1) {
        std::cerr << "Failed to unmap shared memory: " << strerror(errno)
                  << "\n";
        shm_unlink(shm_name_);
    } else {
        std::cout << "Shared memory unmapped successfully. Finished work.\n";
    }

    if (have_to_unlink) {
        if (shm_unlink(shm_name_) == -1)
            std::cerr << "Failed to unlink shared memory: " << strerror(errno)
                      << "\n";
        else
            std::cout << "Unlinked shared memory successfully";
    }
}

void Process::spawn_child_processes() {
    if (active_children_ > 0) {
        sem_wait(&shm_->semaphore);
        shm_->counter.log("Child processes are still running. Skipping spawn.");
        sem_post(&shm_->semaphore);
        return;
    }

    pid_t pidA = fork();
    if (pidA == 0) {
        SubProcessA subProcessA(shm_name_, shm_size_);
        subProcessA.run();
        exit(0);
    } else if (pidA > 0) {
        active_children_++;
    } else {
        std::cerr << "Failed to fork SubProcessA: " << strerror(errno) << "\n";
        return;
    }

    pid_t pidB = fork();
    if (pidB == 0) {
        SubProcessA subProcessB(shm_name_, shm_size_);
        subProcessB.run();
        exit(0);
    } else if (pidB > 0) {
        active_children_++;
    } else {
        std::cerr << "Failed to fork SubProcessB: " << strerror(errno) << "\n";
        return;
    }

    sem_wait(&shm_->semaphore);
    shm_->counter.log("Spawned SubProcessA and SubProcessB.");
    sem_post(&shm_->semaphore);
}

} // namespace moski
