#include "run_processes.h"

std::ostream &lab2::operator<<(std::ostream &os, lab2::ExitCode const &ec) {
  switch (ec) {
  case lab2::ExitCode::Ok:
    os << "Ok";
    break;
  case lab2::ExitCode::Error:
    os << "Error";
    break;
  default:
    os << "";
    break;
  };
  return os;
}

namespace {
void print_errno(const int errno_, const std::string message) {
  std::error_code ec = std::make_error_code(static_cast<std::errc>(errno_));
  std::cerr << message << ": [" << ec.value() << "] " << ec.message() << "\n";
}
} // namespace

const std::vector<lab2::ExitCode>
lab2::run_processes(const int &np, const std::string &program,
                    const std::vector<std::vector<std::string>> &argvv) {
  assert(argvv.size() == np);

  std::vector<ExitCode> exit_codes(np, ExitCode::Error);

#if defined(_WIN32)

#elif defined(__unix__)
  std::vector<int> pids(np);

  for (size_t i = 0; i < np; i++) {
    pid_t pid = fork();
    switch (pid) {
    case -1: {
      int const fork_errno = errno;
      print_errno(fork_errno, "Couldn't fork a new process");
      exit_codes[i] = ExitCode::Error;
    } break;
    case 0: {
      for (size_t i = 0; i < 50; i++)
        std::cout << "-";
      std::cout << "\n";
      std::cout << "Child process #" << i + 1 << " executing...\n";
      for (size_t i = 0; i < 50; i++)
        std::cout << "-";
      std::cout << "\n";

      std::vector<char *> argv(argvv[i].size() + 2);
      argv.front() = const_cast<char *>(program.c_str());
      for (size_t j = 0; j < argvv[i].size(); j++)
        argv[j + 1] = const_cast<char *>(argvv[i][j].c_str());
      argv.back() = nullptr;

      int const res = execvp(argv[0], argv.data());

      if (res == -1) {
        int const exec_errno = errno;
        print_errno(exec_errno, "Couldn't execute the process");
        exit_codes[i] = ExitCode::Error;
      }
    } break;
    default: {
      int status;
      pid_t const res = waitpid(pid, &status, 0);
      if (res == -1) {
        int const fork_errno = errno;
        print_errno(fork_errno, "Couldn't wait for PID");
        exit_codes[i] = ExitCode::Error;
      } else if (WIFEXITED(status))
        exit_codes[i] = static_cast<ExitCode>(WEXITSTATUS(status));
      else {
        kill(pid, SIGTERM);
        exit_codes[i] = ExitCode::Error;
      }
    } break;
    }
  }
#endif

  return exit_codes;
}
