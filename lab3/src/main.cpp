#include "counter.h"
#include "logger.h"
#include "process/process.h"
#include <csignal>
#include <cstring>
#include <iostream>
#include <queue>
#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__)
#include <fcntl.h>    // For O_CREAT, O_RDWR
#include <sys/mman.h> // For shared memory
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#define SHARED_MEM_NAME "MOSKI_COUNTER"
#define SHARED_MEM_SIZE 1024

struct SharedMemoryLayout {
  moski::Counter counter;
  std::queue<pid_t> pid_queue;
};

void cleanup_shared_memory() { shm_unlink(SHARED_MEM_NAME); }

int main() {
  int shm_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    std::cerr << "Failed to open or create shared memory: " << strerror(errno)
              << std::endl;
    return 1;
  }

  if (ftruncate(shm_fd, SHARED_MEM_SIZE) == -1) {
    std::cerr << "Failed to resize shared memory: " << strerror(errno)
              << std::endl;
    cleanup_shared_memory();
    return 1;
  }

  void *shared_mem_ptr = mmap(nullptr, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE,
                              MAP_SHARED, shm_fd, 0);
  if (shared_mem_ptr == MAP_FAILED) {
    std::cerr << "Failed to map shared memory: " << strerror(errno)
              << std::endl;
    cleanup_shared_memory();
    return 1;
  }

  close(shm_fd);

  auto *shared_layout = static_cast<SharedMemoryLayout *>(shared_mem_ptr);

  bool is_leader = false;
  if (shared_layout->pid_queue.empty()) {
    is_leader = true;
    std::cout << "This process is the leader." << std::endl;
  } else {
    std::cout << "This process is a follower." << std::endl;
  }
  shared_layout->pid_queue.push(getpid());

  moski::Counter &shared_counter = shared_layout->counter;

  moski::Process process(shared_counter, is_leader);

  std::signal(SIGINT, [](int) {
    std::cout << "Process interrupted. Cleaning up..." << std::endl;
    cleanup_shared_memory();
    exit(0);
  });

  // Run the process
  process.run();

  // Clean up shared memory if this process is the leader
  if (is_leader) {
    cleanup_shared_memory();
  }

  // Unmap the shared memory
  if (munmap(shared_mem_ptr, SHARED_MEM_SIZE) == -1) {
    std::cerr << "Failed to unmap shared memory: " << strerror(errno)
              << std::endl;
  }

  return 0;
}
