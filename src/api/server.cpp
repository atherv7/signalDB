#include "server.h"

#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/role.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/websocket/stream_base.hpp>
#include <thread>
#include <utility>

#include "helpers.h"
#include "routes.h"
#include "storage/models.h"

Server::Server(std::shared_ptr<Storage> storage)
    : storage(std::move(storage)), acceptor(ioc, {this->address, this->port}) {}

Server::~Server() {
  shutdown();
}

void Server::shutdown() {
  beast::error_code ec;
  auto ec_result = this->acceptor.close(ec);
  if (ec_result) {
    std::cerr << "Acceptor failed to close: " << ec.message() << "\n";
  }
  this->running = false;
  this->ioc.stop();

  for (auto& t : this->threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}

void Server::run() {
  this->do_accept();
  this->ioc.run();
}

void Server::do_accept() {
  this->acceptor.async_accept([this](beast::error_code ec, tcp::socket socket) {
    if (not ec) {
      threads.emplace_back(&Server::session, this, std::move(socket));
    }
  });
}

void Server::session(tcp::socket socket) {
  beast::error_code ec;
  beast::flat_buffer buffer;
  http::request<http::string_body> req;
  http::read(socket, buffer, req, ec);

  if (ec) {
    return;
  }

  if (websocket::is_upgrade(req)) {
    this->websocket_conn(req, std::move(socket));
  } else {
    this->handle_http(std::move(req), socket);
  }
}

void Server::handle_http(http::request<http::string_body>&& req, tcp::socket& socket) {
  auto const target = req.target();

  if (target == "/api/store" && req.method() == http::verb::post) {
    return routes::post_request(std::move(req), socket, this->storage);
  }

  http::response<http::string_body> res{http::status::not_found, req.version()};
  res.body() = "{\"error\":\"Route not found\"}";
  res.prepare_payload();
  http::write(socket, res);
}

void Server::websocket_conn(const http::request<http::string_body>& req, tcp::socket socket) {
  try {
    websocket::stream<tcp::socket> ws{std::move(socket)};
    ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
    ws.accept(req);

    for (;;) {
      beast::flat_buffer buffer;
      beast::error_code ec;
      ws.read(buffer, ec);

      if (ec == websocket::error::closed) {
        break;
      } else if (ec) {
        std::cerr << "Read error: " << ec.message() << "\n";
        break;
      }

      std::string json_string = beast::buffers_to_string(buffer.data());

      std::vector<models::Entry> entries = helpers::parse_json_for_entries(json_string);

      storage->write_ahead_insert(entries);

      const std::string success_msg{"{\"status\": \"success\", \"message\": \"Data stored\"}"};
      ws.text(true);
      ws.write(net::buffer(success_msg), ec);

      if (ec) {
        std::cerr << "Write error: " << ec.message() << "\n";
        break;
      }
    }
  } catch (beast::system_error const& e) {
    std::cerr << "Unexpected exception: " << e.what() << "\n";
  }
}
