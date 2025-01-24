#pragma once

#include "process/process.h"

namespace moski {

class SubProcessB : public Process {
  public:
    SubProcessB(const char *shm_name, std::size_t shm_size);

    void run();
};

} // namespace moski
