#include "process/process.h"
#include <csignal>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__)
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

constexpr const char *shm_name = "/MOSKI_COUNTER";
constexpr std::size_t shm_size = sizeof(moski::SharedMemoryLayout);

moski::SharedMemoryLayout *init_shm() {
    std::cout << "Shared memory init...";
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
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

    return static_cast<moski::SharedMemoryLayout *>(shm_ptr);
}

int main() {
    moski::SharedMemoryLayout *shm = init_shm();

    moski::Process process(shm_name, shm_size, shm);

    try {
        process.run();
    } catch (...) {
        shm_unlink(shm_name);
        throw;
    }

    return 0;
}
