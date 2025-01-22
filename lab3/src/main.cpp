#include "logger.h"
#include "process/process.h"
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <cstring>
#include <iostream>
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
constexpr std::size_t shm_size = sizeof(moski::SharedMemoryLayout) + 1024;

int main() {
    std::cout << "Starting process\n";

    try {
        moski::Process process(shm_name, shm_size);
        process.run();
    } catch (...) {
        shm_unlink(shm_name);
        throw;
    }

    return 0;
}
