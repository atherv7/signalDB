#include "session.h"

#include <boost/asio/buffer.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/role.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/websocket/rfc6455.hpp>
#include <boost/beast/websocket/stream_base.hpp>
#include <memory>
#include <utility>

#include "helpers.h"
#include "routes.h"

HttpSession::HttpSession(tcp::socket&& socket,
                         std::shared_ptr<Storage> storage,
                         std::shared_ptr<SessionManager> manager)
    : stream(std::move(socket)), storage(std::move(storage)), manager(std::move(manager)) {}

void HttpSession::run() {
  this->do_read();
}

void HttpSession::stop() {
  beast::error_code ec;
  this->stream.socket().close(ec);

  if (ec) {
    std::cerr << "Close error: " << ec.message() << "\n";
  }
}

void HttpSession::do_read() {
  this->req = {};
  http::async_read(this->stream,
                   this->buffer,
                   this->req,
                   beast::bind_front_handler(&HttpSession::on_read, shared_from_this()));
}

void HttpSession::on_read(beast::error_code ec, std::size_t) {
  if (ec) {
    return;
  }

  if (websocket::is_upgrade(this->req)) {
    auto ws = std::make_shared<WebsocketSession>(
        this->stream.release_socket(), this->storage, this->manager);
    ws->run_upgraded(std::move(this->req));
    return;
  }

  this->handle_http();
}

void HttpSession::handle_http() {
  auto const target = this->req.target();

  if (target == "/api/store" && this->req.method() == http::verb::post) {
    return routes::post_request(std::move(this->req), this->stream.socket(), this->storage);
  }

  auto res = std::make_shared<http::response<http::string_body>>(http::status::not_found,
                                                                 this->req.version());

  res->set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res->set(http::field::content_type, "application/json");
  res->body() = "{\"error\":\"Route not found\"}";
  res->prepare_payload();

  http::async_write(
      this->stream,
      *res,
      [self = shared_from_this(), res](beast::error_code ec, std::size_t bytes_transferred) {
        if (ec) {
          return;
        }

        self->stop();
      });
}

WebsocketSession::WebsocketSession(tcp::socket&& socket,
                                   std::shared_ptr<Storage> storage,
                                   std::shared_ptr<SessionManager> manager)
    : ws(std::move(socket)), storage(std::move(storage)), manager(std::move(manager)) {}

void WebsocketSession::run_upgraded(http::request<http::string_body> req) {
  this->ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
  this->ws.async_accept(
      req, beast::bind_front_handler(&WebsocketSession::on_accept, shared_from_this()));
}

void WebsocketSession::run() {}

void WebsocketSession::stop() {
  beast::error_code ec;
  beast::get_lowest_layer(this->ws).socket().close(ec);

  if (ec) {
    std::cerr << "Close error: " << ec.message() << "\n";
  }
}

void WebsocketSession::on_accept(beast::error_code ec) {
  if (!ec) {
    this->do_read();
  }
}

void WebsocketSession::do_read() {
  this->buffer.consume(this->buffer.size());
  this->ws.async_read(this->buffer,
                      beast::bind_front_handler(&WebsocketSession::on_read, shared_from_this()));
}

void WebsocketSession::on_read(beast::error_code ec, std::size_t) {
  if (ec) {
    return;
  }

  std::string json_string = beast::buffers_to_string(this->buffer.data());
  auto entries = helpers::parse_json_for_entries(json_string);
  this->storage->write_ahead_insert(entries);

  this->ws.async_write(net::buffer("{\"status\":\"success\"}"),
                       beast::bind_front_handler(&WebsocketSession::on_write, shared_from_this()));
}

void WebsocketSession::on_write(beast::error_code ec, std::size_t) {
  if (!ec) {
    this->do_read();
  }
}
