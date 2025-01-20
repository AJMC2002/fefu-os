
#include "counter.h"
#include "logger.h"
#include <atomic>
#include <iostream>
#include <thread>
#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__)
#include <unistd.h>
#endif

std::atomic<bool> keep_running(true);
std::atomic<bool> can_spawn(true);
std::atomic<int> active_children(0);

void start_timer(moski::Counter &counter) {
  while (keep_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    counter.increment(1);
  }
}

void log_counter(moski::Counter &counter, const std::string &path) {
  while (keep_running) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    counter.log(path);
  }
}

void spawn_child_processes(moski::Counter &counter,
                           const std::string &log_file_path) {
  while (keep_running) {
    std::this_thread::sleep_for(std::chrono::seconds(3));

    if (active_children.load() > 0) {
      std::ofstream log_file(log_file_path, std::ios::app);
      log_file
          << "Another instance is still running, skipping child creation.\n";
      continue;
    }

    // Spawn child process 1
    active_children.fetch_add(1);
    std::thread(
        [](moski::Counter &counter, const std::string &log_file_path) {
          moski::Logger logger(log_file_path);
          logger.log("Child 1 started, PID: " + std::to_string(getpid()));

          counter.increment(10);
          logger.log("Child 1 exiting, counter value: " +
                     std::to_string(counter.get_value()));

          active_children.fetch_sub(1);
        },
        std::ref(counter), log_file_path)
        .detach();

    // Spawn child process 2
    active_children.fetch_add(1);
    std::thread(
        [](moski::Counter &counter, const std::string &log_file_path) {
          moski::Logger logger(log_file_path);
          logger.log("Child 2 started, PID: " + std::to_string(getpid()));

          counter.set_value(counter.get_value() * 2);
          std::this_thread::sleep_for(std::chrono::seconds(2));
          counter.set_value(counter.get_value() / 2);

          logger.log("Child 2 exiting, counter value: " +
                     std::to_string(counter.get_value()));

          active_children.fetch_sub(1);
        },
        std::ref(counter), log_file_path)
        .detach();
  }
}

void handle_user_input(moski::Counter &counter) {
  while (keep_running) {
    int new_value;
    std::cout << "Enter new counter value: ";
    std::cin >> new_value;

    if (std::cin.fail()) {
      if (std::cin.eof()) {
        std::cout << "\n";
        keep_running = false;
        break;
      } else {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter an integer.\n";
        continue;
      }
    }
    counter.set_value(new_value);
    std::cout << "Counter updated to " << new_value << "\n";
  }
}

int main() {
  const std::string log_file_path = "program.log";
  moski::Counter counter(0);
  moski::Logger logger(log_file_path);

  logger.log("Program started, PID: " + std::to_string(getpid()));

  std::thread timer_thread(start_timer, std::ref(counter));
  std::thread logger_thread(log_counter, std::ref(counter), log_file_path);
  std::thread spawner_thread(spawn_child_processes, std::ref(counter),
                             log_file_path);
  std::thread user_input_thread(handle_user_input, std::ref(counter));

  timer_thread.join();
  logger_thread.join();
  spawner_thread.join();
  user_input_thread.join();

  logger.log("Program exiting, PID: " + std::to_string(getpid()));

  return 0;
}
