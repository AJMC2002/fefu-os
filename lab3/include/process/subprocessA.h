#pragma once

#include "process/process.h"

namespace moski {

class SubProcessA : Process {
  public:
    SubProcessA(const char *shm_name, std::size_t shm_size);

    void run();
};

} // namespace moski
