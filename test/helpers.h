#pragma once

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <chrono>
#include <filesystem>
#include <vector>

#include "storage/models.h"

namespace fs = std::filesystem;
namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace helpers {
struct File {
  std::string& path;

  [[nodiscard]] auto wait_for_file(
      std::chrono::seconds timeout = std::chrono::seconds(30),
      std::chrono::milliseconds poll_interval = std::chrono::milliseconds(1000)) const -> bool;

  ~File() {
    if (std::filesystem::exists(path)) {
      std::cout << "removing file" << std::endl;
      std::filesystem::remove(path);
    }
  }
};

auto post_request(const std::vector<models::Entry>& entries) -> bool;
}  // namespace helpers
