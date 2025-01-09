#pragma once

#include <cassert>
#include <iostream>
#include <ostream>
#include <vector>
#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__)
#include <algorithm>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace lab2 {
enum class ExitCode : int { Ok = 0, Error = 1 };

std::ostream &operator<<(std::ostream &os, const ExitCode &ec);

const std::vector<ExitCode>
run_processes(const int &np, const std::string &program,
              const std::vector<std::vector<std::string>> &argvv);
} // namespace lab2
