#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <memory>

#include "storage/storage.h"

namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

namespace routes {
void post_request(http::request<http::string_body>&& req,
                  tcp::socket& socket,
                  const std::shared_ptr<Storage>& storage);
}
