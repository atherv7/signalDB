#include "helpers.h"

#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/websocket.hpp>
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

auto post_ws(const std::vector<models::Entry>& entries) -> bool {
  try {
    net::io_context ioc;
    tcp::resolver resolver{ioc};
    websocket::stream<tcp::socket> ws{ioc};

    auto const results = resolver.resolve("127.0.0.1", "8000");

    net::connect(ws.next_layer(), results.begin(), results.end());
    ws.handshake("localhost", "/");

    for (const auto& entry : entries) {
      std::vector<models::Entry> entry_arr{entry};
      nlohmann::json data = entry_arr;
      std::string data_json_str = data.dump();
      ws.write(net::buffer(data_json_str));

      beast::flat_buffer buffer;
      ws.read(buffer);

      std::cout << beast::buffers_to_string(buffer.data()) << "\n";
    }
    ws.close(websocket::close_code::normal);

    return true;
  } catch (const std::exception& e) {
    std::cerr << "Client error: " << e.what() << "\n";
  }

  return false;
}
}  // namespace helpers
