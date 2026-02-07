#pragma once

#include <algorithm>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Server {
 public:
  Server();

  void run();

 private:
  const net::ip::address address = net::ip::make_address("0.0.0.0");
  const unsigned short port = static_cast<unsigned short>(8000);
};
