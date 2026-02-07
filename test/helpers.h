#pragma once

#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

namespace helpers {
struct File {
  std::string& path;

  [[nodiscard]] auto wait_for_file(
      std::chrono::seconds timeout = std::chrono::seconds(30),
      std::chrono::milliseconds poll_interval = std::chrono::milliseconds(1000)) const -> bool;

  ~File() {
    if (std::filesystem::exists(path)) {
      std::filesystem::remove(path);
    }
  }
};
}  // namespace helpers
