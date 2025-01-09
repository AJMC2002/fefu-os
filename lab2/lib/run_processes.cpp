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
    os << "EXIT CODE NOT RECOGNIZED";
    break;
  };
  return os;
}

namespace {
void print_error(const int errid, const std::string message) {
#if defined(_WIN32)
  if (errid == 0)
    return;

  LPSTR errmessage_buf = nullptr;

  size_t size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, errid, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPSTR)&errmessage_buf, 0, NULL);

  std::string errmessage(errmessage_buf, size);

  LocalFree(errmessage_buf);

  std::cerr << message << ": [" << errid << "] " << errmessage << "\n";
#elif defined(__unix__)
  std::error_code ec = std::make_error_code(static_cast<std::errc>(errid));
  std::cerr << message << ": [" << ec.value() << "] " << ec.message() << "\n";
#endif
}
} // namespace

const std::vector<lab2::ExitCode>
lab2::run_processes(const int &np, const std::string &program,
                    const std::vector<std::vector<std::string>> &argvv) {
  assert(argvv.size() == np);

#if defined(__unix__)
  std::vector<int> pids(np);
#endif
  std::vector<ExitCode> exit_codes(np, ExitCode::Error);

  for (size_t i = 0; i < np; i++) {
    for (size_t i = 0; i < 52; i++)
      std::cout << "-";
    std::cout << "\n";
    std::cout << "Child process #" << i + 1 << " executing...\n";
    for (size_t i = 0; i < 52; i++)
      std::cout << "-";
    std::cout << "\n";

#if defined(_WIN32)
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::ostringstream os;
    os << "cmd.exe /c ";
    os << program;
    for (std::string arg : argvv[i])
      os << " " << arg;
    std::string cmdline = os.str();

    if (!CreateProcessA(NULL, const_cast<char *>(cmdline.c_str()), NULL, NULL,
                        FALSE, 0, NULL, NULL, &si, &pi))
      print_error(GetLastError(), "CreateProcess failed");

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code;
    if (GetExitCodeProcess(pi.hProcess, &exit_code))
      if (exit_code == STATUS_PENDING)
        std::cerr << "Failed to get exit code. Process still running.\n";
      else
        exit_codes[i] = static_cast<ExitCode>(exit_code);
    else
      print_error(GetLastError(), "Failed to get exit code");

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
#elif defined(__unix__)
    pid_t pid = fork();
    switch (pid) {
    case -1: {
      int const fork_errno = errno;
      print_error(fork_errno, "Couldn't fork a new process");
      exit_codes[i] = ExitCode::Error;
    } break;
    case 0: {
      std::vector<char *> argv(argvv[i].size() + 2);
      argv.front() = const_cast<char *>(program.c_str());
      for (size_t j = 0; j < argvv[i].size(); j++)
        argv[j + 1] = const_cast<char *>(argvv[i][j].c_str());
      argv.back() = nullptr;

      int const res = execvp(argv[0], argv.data());

      if (res == -1) {
        int const exec_errno = errno;
        print_error(exec_errno, "Couldn't execute the process");
        exit_codes[i] = ExitCode::Error;
      }
    } break;
    default: {
      int status;
      pid_t const res = waitpid(pid, &status, 0);
      if (res == -1) {
        int const fork_errno = errno;
        print_error(fork_errno, "Couldn't wait for PID");
        exit_codes[i] = ExitCode::Error;
      } else if (WIFEXITED(status))
        exit_codes[i] = static_cast<ExitCode>(WEXITSTATUS(status));
      else {
        kill(pid, SIGTERM);
        exit_codes[i] = ExitCode::Error;
      }
    } break;
    }
#endif
  }
  return exit_codes;
}
