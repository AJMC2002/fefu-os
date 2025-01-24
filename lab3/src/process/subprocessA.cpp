#include "process/subprocessA.h"

namespace moski {

SubProcessA::SubProcessA(const char *shm_name, std::size_t shm_size)
    : Process(shm_name, shm_size) {}

void SubProcessA::run() {
    sem_wait(&shm_->semaphore);
    shm_->counter.log("SubProcessA started.");
    sem_post(&shm_->semaphore);

    sem_wait(&shm_->semaphore);
    shm_->counter += 10;
    sem_post(&shm_->semaphore);

    sem_wait(&shm_->semaphore);
    shm_->counter.log("SubProcessA ended.");
    sem_post(&shm_->semaphore);
}

} // namespace moski
