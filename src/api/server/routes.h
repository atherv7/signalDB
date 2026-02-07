#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

namespace routes {
void post_request(http::request<http::string_body>&& req);
}
