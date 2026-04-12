#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <cstddef>
#include <memory>

#include "storage/storage.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class SessionManager;

class BaseSession : public std::enable_shared_from_this<BaseSession> {
 public:
  virtual ~BaseSession() = default;
  virtual void run() = 0;
  virtual void stop() = 0;
};

class HttpSession : public BaseSession {
 public:
  HttpSession(tcp::socket&& socket,
              std::shared_ptr<Storage> storage,
              std::shared_ptr<SessionManager> manager);

  void run() override;
  void stop() override;

 private:
  beast::tcp_stream stream;
  beast::flat_buffer buffer;
  std::shared_ptr<Storage> storage;
  http::request<http::string_body> req;
  std::shared_ptr<SessionManager> manager;

  void do_read();
  void on_read(beast::error_code ec, std::size_t bytes_transferred);
  void handle_http();
};

class WebsocketSession : public BaseSession {
 public:
  WebsocketSession(tcp::socket&& socket,
                   std::shared_ptr<Storage> storage,
                   std::shared_ptr<SessionManager> manager);
  void run() override;
  void run_upgraded(http::request<http::string_body> req);
  void stop() override;

 private:
  websocket::stream<beast::tcp_stream> ws;
  beast::flat_buffer buffer;
  std::shared_ptr<Storage> storage;
  std::shared_ptr<SessionManager> manager;

  void on_accept(beast::error_code ec);
  void do_read();
  void on_read(beast::error_code ec, std::size_t bytes_transferred);
  void on_write(beast::error_code ec, std::size_t bytes_transferred);
};
