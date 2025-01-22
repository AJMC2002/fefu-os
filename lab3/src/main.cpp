#include "process/process.h"
#include "signal_handler.h"

#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__)
#include <sys/mman.h>
#endif

constexpr const char *shm_name = "/MOSKI_COUNTER";
constexpr std::size_t shm_size = sizeof(moski::SharedMemoryLayout) + 1024;

int main() {
    std::cout << "Starting process\n";

    auto &signal_handler = moski::SignalHandler::get_instance();
    signal_handler.register_signals();

    try {
        moski::Process process(shm_name, shm_size);

        signal_handler.set_process(&process);

        process.run();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception occurred.\n";
        return 1;
    }

    return 0;
}
