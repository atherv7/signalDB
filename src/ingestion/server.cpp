#include "server.h"

#include <boost/asio/ip/address.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/error.hpp>
#include <utility>

void SessionManager::add(std::shared_ptr<BaseSession> session) {
  std::lock_guard<std::mutex> lock(this->mutex);
  this->sessions.insert(std::move(session));
}

void SessionManager::remove(std::shared_ptr<BaseSession> session) {
  std::lock_guard<std::mutex> lock(this->mutex);
  this->sessions.erase(session);
}

void SessionManager::close_all() {
  std::lock_guard<std::mutex> lock(this->mutex);
  for (auto& s : this->sessions) {
    s->stop();
  }
  this->sessions.clear();
}

Server::Server(std::shared_ptr<Storage> storage, net::io_context& ioc)
    : ioc(ioc),
      acceptor(ioc, {net::ip::make_address("0.0.0.0"), 8000}),
      storage(std::move(storage)),
      session_manager(std::make_shared<SessionManager>()) {}

void Server::run() {
  this->do_accept();
}

void Server::do_accept() {
  this->acceptor.async_accept(net::make_strand(this->ioc),
                              [this](beast::error_code ec, tcp::socket socket) {
                                if (!ec) {
                                  auto session = std::make_shared<HttpSession>(
                                      std::move(socket), this->storage, this->session_manager);
                                  this->session_manager->add(session);
                                  session->run();
                                }
                                do_accept();
                              });
}

void Server::shutdown() {
  this->acceptor.close();
  this->session_manager->close_all();
}
