#include "helpers.h"

#include <boost/asio/io_context.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/version.hpp>
#include <chrono>
#include <exception>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <thread>
#include <vector>

#include "storage/models.h"

namespace helpers {
auto File::wait_for_file(std::chrono::seconds timeout,
                         std::chrono::milliseconds poll_interval) const -> bool {
  const auto deadline = std::chrono::steady_clock::now() + timeout;

  while (std::chrono::steady_clock::now() < deadline) {
    if (fs::exists(this->path)) {
      return true;
    }
    std::this_thread::sleep_for(poll_interval);
  }

  return false;
}

auto post_request(const std::vector<models::Entry>& entries) -> bool {
  try {
    net::io_context ioc;
    tcp::resolver resolver{ioc};
    beast::tcp_stream stream{ioc};
    nlohmann::json entries_json = entries;

    auto const results = resolver.resolve("127.0.0.1", "8000");
    stream.connect(results);

    http::request<http::string_body> req{http::verb::post, "/api/store", 11};
    req.set(http::field::host, "127.0.0.1");
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::content_type, "application/json");
    req.body() = entries_json.dump();
    req.prepare_payload();

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    return res.result_int() >= 200 && res.result_int() < 300;
  } catch (const std::exception& e) {
    std::cerr << "Request failed: " << e.what() << "\n";
    return false;
  }
}
}  // namespace helpers
