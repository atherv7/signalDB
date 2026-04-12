#pragma once

#include <boost/asio/io_context.hpp>
#include <memory>
#include <mutex>
#include <set>

#include "session.h"

class SessionManager {
 public:
  void add(std::shared_ptr<BaseSession> session);
  void remove(std::shared_ptr<BaseSession> session);
  void close_all();

 private:
  std::mutex mutex;
  std::set<std::shared_ptr<BaseSession>> sessions;
};

class Server : public std::enable_shared_from_this<Server> {
 public:
  Server(std::shared_ptr<Storage> storage, net::io_context& ioc);
  void run();
  void shutdown();

 private:
  net::io_context& ioc;
  tcp::acceptor acceptor;
  std::shared_ptr<Storage> storage;
  std::shared_ptr<SessionManager> session_manager;

  void do_accept();
};
