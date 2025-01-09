#include "run_processes.h"
#include "gtest/gtest.h"
#include <filesystem>

#define STRING(x) #x
#define XSTRING(x) std::string(STRING(x))

void print_exit_codes(const int &np, const std::string &program,
                      const std::vector<lab2::ExitCode> &exit_codes) {
  assert(exit_codes.size() == np);

  std::cout << "+";
  for (int i = 0; i < 50; i++)
    std::cout << "-";
  std::cout << "+\n";

  std::cout << "Program: " << program << "\n";

  std::cout << "+";
  for (int i = 0; i < 50; i++)
    std::cout << "-";
  std::cout << "+\n";

  for (int i = 0; i < exit_codes.size(); i++)
    std::cout << "Process #" << i + 1 << ": " << exit_codes[i] << "\n";

  std::cout << "+";
  for (int i = 0; i < 50; i++)
    std::cout << "-";
  std::cout << "+\n";
}

TEST(RunProcessesTest, NoArgs) {
  int np = 1;
  std::string program;
#if defined(_WIN32)
  program = "dir";
#elif defined(__unix__)
  program = "ls";
#endif
  std::vector<std::vector<std::string>> argvv(np);

  std::vector<lab2::ExitCode> exit_codes =
      lab2::run_processes(np, program, argvv);
  print_exit_codes(np, program, exit_codes);

  for (int i = 0; i < np; i++)
    EXPECT_EQ(exit_codes[i], lab2::ExitCode::Ok);
}

TEST(RunProcessesTest, ManyArgs) {
  int np = 2;
  std::string program = "ping";
  std::vector<std::vector<std::string>> argvv;
#if defined(_WIN32)
  argvv =
      std::vector<std::vector<std::string>>(np, {"", "-n", "1", "-w", "2000"});
#elif defined(__unix__)
  argvv = std::vector<std::vector<std::string>>(np, {"", "-c", "1", "-W", "5"});
#endif
  argvv[0][0] = "google.com";
  argvv[1][0] = "10.255.255.255";

  std::vector<lab2::ExitCode> exit_codes =
      lab2::run_processes(np, program, argvv);
  print_exit_codes(np, program, exit_codes);

  EXPECT_EQ(exit_codes[0], lab2::ExitCode::Ok);
  EXPECT_EQ(exit_codes[1], lab2::ExitCode::Error);
}

TEST(RunProcessesTest, ManyProcesses) {
  int np = 10;
  std::string program = "echo";
  std::vector<std::vector<std::string>> argvv(np, {"\"Hi\""});

  std::vector<lab2::ExitCode> exit_codes =
      lab2::run_processes(np, program, argvv);
  print_exit_codes(np, program, exit_codes);

  for (int i = 0; i < np; i++)
    EXPECT_EQ(exit_codes[i], lab2::ExitCode::Ok);
}

TEST(RunProcessesTest, CatFiles) {
  int np = 2;
  std::string program;
#if defined(_WIN32)
  program = "type";
#elif defined(__unix__)
  program = "cat";
#endif
  std::vector<std::vector<std::string>> argvv(np, {""});
  argvv[0][0] = std::filesystem::path(XSTRING(SOURCE_ROOT) + "/CMakeLists.txt")
                    .make_preferred()
                    .string();
  argvv[1][0] = std::filesystem::path(XSTRING(SOURCE_ROOT) + "/None.txt")
                    .make_preferred()
                    .string();

  std::vector<lab2::ExitCode> exit_codes =
      lab2::run_processes(np, program, argvv);
  print_exit_codes(np, program, exit_codes);

  EXPECT_EQ(exit_codes[0], lab2::ExitCode::Ok);
  EXPECT_EQ(exit_codes[1], lab2::ExitCode::Error);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
