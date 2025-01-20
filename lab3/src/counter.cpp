#include "counter.h"

namespace moski {

Counter::Counter(int initial_value, const std::string &log_path)
    : value_(initial_value), logger_(log_path) {}

int Counter::get_value() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return value_;
}

void Counter::set_value(int const &n) {
  std::lock_guard<std::mutex> lock(mutex_);
  value_ = n;
}

void Counter::operator=(int const &n) {
  std::lock_guard<std::mutex> lock(mutex_);
  value_ = n;
}

void Counter::operator+=(int const &n) {
  std::lock_guard<std::mutex> lock(mutex_);
  value_ += n;
}

void Counter::operator-=(int const &n) {
  std::lock_guard<std::mutex> lock(mutex_);
  value_ -= n;
}

void Counter::operator*=(int const &n) {
  std::lock_guard<std::mutex> lock(mutex_);
  value_ *= n;
}

void Counter::operator/=(int const &n) {
  std::lock_guard<std::mutex> lock(mutex_);
  value_ /= n;
}

void Counter::log(const std::string &message) const { logger_.log(message); }

} // namespace moski
