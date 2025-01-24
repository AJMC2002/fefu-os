#include "counter.h"
#include <string>

namespace moski {

Counter::Counter(int initial_value) : value_(initial_value), logger_() {}

int Counter::get_value() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return value_;
}

void Counter::set_value(int const &n) {
    std::lock_guard<std::mutex> lock(mutex_);
    logger_.log("Value set to: " + std::to_string(n) +
                " (Previous value: " + std::to_string(value_) + ").");
    value_ = n;
}

void Counter::operator=(int const &n) {
    std::lock_guard<std::mutex> lock(mutex_);
    logger_.log("Value set to: " + std::to_string(n) +
                " (Previous value: " + std::to_string(value_) + ").");
    value_ = n;
}

void Counter::operator+=(int const &n) {
    std::lock_guard<std::mutex> lock(mutex_);
    logger_.log("Increased value " + std::to_string(value_) + " by " +
                std::to_string(n) + ". Result: " + std::to_string(value_ + n) +
                ".");
    value_ += n;
}

void Counter::operator-=(int const &n) {
    std::lock_guard<std::mutex> lock(mutex_);
    logger_.log("Decreased value " + std::to_string(value_) + " by " +
                std::to_string(n) + ". Result: " + std::to_string(value_ - n) +
                ".");
    value_ -= n;
}

void Counter::operator*=(int const &n) {
    std::lock_guard<std::mutex> lock(mutex_);
    logger_.log("Increased value " + std::to_string(value_) +
                " by a factor of " + std::to_string(n) +
                ". Result: " + std::to_string(value_ * n) + ".");
    value_ *= n;
}

void Counter::operator/=(int const &n) {
    std::lock_guard<std::mutex> lock(mutex_);
    logger_.log("Decreased value " + std::to_string(value_) +
                " by a factor of " + std::to_string(n) +
                ". Result: " + std::to_string(value_ / n) + ".");
    value_ /= n;
}

void Counter::log(const std::string &message) const { logger_.log(message); }

void Counter::cleanup() { logger_.cleanup(); }

} // namespace moski
