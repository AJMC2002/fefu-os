#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#ifdef _WIN32
#include <thread>
#include <windows.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

void sleep_ms(int milliseconds) {
#ifdef _WIN32
  Sleep(milliseconds);
#else
  usleep(milliseconds * 1000);
#endif
}

void simulate_process(int process_id, int bucket_size,
                      const std::vector<int> &numbers) {
  sleep_ms(100 * numbers.size()); // simulate time taken to process

  std::ostringstream oss;
  bool is_error = numbers.size() != bucket_size;

  oss << "Process " << process_id << ": "
      << (!is_error ? "    Equal - " : "Not Equal - ");

  oss << "[ ";
  for (size_t i = 0; i < numbers.size(); ++i) {
    oss << numbers[i] << (i + 1 < numbers.size() ? " " : "");
  }
  oss << " ]\n";

  std::cout << oss.str();

  exit(!is_error ? 0 : 1);
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0] << " -n <number> -np <processes>\n";
    return 1;
  }

  int n = 0, np = 0;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-n" && i + 1 < argc) {
      n = std::atoi(argv[++i]);
    } else if (arg == "-np" && i + 1 < argc) {
      np = std::atoi(argv[++i]);
    }
  }

  if (n <= 0 || np <= 0) {
    std::cerr << "Both -n and -np must be positive integers greater than 0.\n";
    return 1;
  }

  int bucket_size = (int)std::ceil((double)n / (double)np);

  std::vector<std::vector<int>> process_buckets(np);
  for (int i = 0; i < n; ++i) {
    process_buckets[i / bucket_size].push_back(i + 1);
  }

#ifdef _WIN32
  std::vector<std::thread> threads;
  std::vector<int> exit_codes(np, 0);

  for (int i = 0; i < np; ++i) {
    threads.emplace_back([&, i]() {
      try {
        simulate_process(i, bucket_size, process_buckets[i]);
      } catch (...) {
        exit_codes[i] = 1;
      }
    });
  }
  for (auto &t : threads) {
    t.join();
  }

  for (int i = 0; i < 50; ++i)
    std::cout << "-";
  std::cout << "\n";

  for (int i = 0; i < np; ++i) {
    std::cout << "Process " << i << ": "
              << (exit_codes[i] == 0 ? "Success" : "Error") << "\n";
  }

#else
  std::vector<int> pids, exit_codes;

  for (int i = 0; i < np; ++i) {
    pid_t pid = fork();
    if (pid == 0) {
      simulate_process(i, bucket_size, process_buckets[i]);
      exit(0);
    } else
      pids.push_back(pid);
  }

  for (int i = 0; i < np; ++i) {
    int status;
    waitpid(pids[i], &status, 0);
    if (WIFEXITED(status)) {
      int exit_code = WEXITSTATUS(status);
      exit_codes.push_back(exit_code);
    }
  }

  for (int i = 0; i < 50; ++i)
    std::cout << "-";
  std::cout << "\n";

  for (int i = 0; i < np; i++) {
    std::cout << "Process " << i << ": "
              << (exit_codes[i] == 0 ? "Success" : "Error") << "\n";
  }

  std::cout << "RESULT: " << n << " is ";
  if (std::any_of(exit_codes.begin(), exit_codes.end(),
                  [](int &x) { return x == 1; }))
    std::cout << "not";
  std::cout << " divisible by " << np << ".";
#endif

  return 0;
}
