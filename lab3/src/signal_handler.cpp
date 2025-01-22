#include "signal_handler.h"

#include <iostream>

namespace moski {

SignalHandler &SignalHandler::get_instance() {
    static SignalHandler instance;
    return instance;
}

void SignalHandler::register_signals() {
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);
}

void SignalHandler::set_process(Process *process) { process_ = process; }

void SignalHandler::handle_signal(int signal) {
    std::cout << "Caught signal " << signal << ". Cleaning up...\n";

    auto &instance = get_instance();

    if (instance.process_) {
        instance.process_->~Process();
    }

    std::exit(signal);
}

} // namespace moski
