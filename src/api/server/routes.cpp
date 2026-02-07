#include "routes.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <exception>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace routes {
void post_request(http::request<http::string_body>&& req) {
  try {
    auto data = json::parse(req.body());

  } catch (const std::exception& e) {
  }
}
}  // namespace routes
