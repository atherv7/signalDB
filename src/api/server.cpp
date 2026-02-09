#include "server.h"

#include <boost/beast/core/error.hpp>
#include <boost/beast/http/write.hpp>
#include <thread>
#include <utility>

#include "routes.h"

Server::Server(std::shared_ptr<Storage> storage)
    : storage(std::move(storage)), acceptor(ioc, {this->address, this->port}) {}

Server::~Server() {
  shutdown();
}

void Server::shutdown() {
  beast::error_code ec;
  this->acceptor.close(ec);
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
