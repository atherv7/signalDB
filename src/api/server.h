#pragma once

#include <boost/asio/dispatch.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/websocket.hpp>
#include <memory>
#include <vector>

#include "storage/storage.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Server {
 public:
  Server(std::shared_ptr<Storage> storage);
  ~Server();

  void run();

  void shutdown();

 private:
  const net::ip::address address = net::ip::make_address("0.0.0.0");
  const unsigned short port = static_cast<unsigned short>(8000);
  net::io_context ioc{1};
  tcp::acceptor acceptor;
  std::vector<std::thread> threads;
  std::atomic<bool> running{true};
  std::shared_ptr<Storage> storage;

  void handle_http(http::request<http::string_body>&& req, tcp::socket& socket);

  void session(tcp::socket socket);
};
