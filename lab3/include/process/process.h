#include "counter.h"
#include <atomic>

namespace moski {
class Process {
public:
  void run();

private:
  bool is_leader;
  std::atomic<bool> is_running;
  std::atomic<bool> can_spawn;
  std::atomic<int> active_children;

  void timer(Counter &counter);
  void spawner(Counter &counter);
};
} // namespace moski
