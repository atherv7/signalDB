#include "helpers.h"
#include <chrono>
#include <filesystem>
#include <thread>

namespace helpers {
auto File::wait_for_file(std::chrono::seconds timeout,
                         std::chrono::milliseconds poll_interval) const
    -> bool {
  const auto deadline = std::chrono::steady_clock::now() + timeout;

  while (std::chrono::steady_clock::now() < deadline) {
    if (fs::exists(this->path)) {
      return true;
    }
    std::this_thread::sleep_for(poll_interval);
  }

  return false;
}
} // namespace helpers
