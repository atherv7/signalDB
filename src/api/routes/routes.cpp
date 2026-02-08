#include "routes.h"

#include <boost/beast/http/impl/write.hpp>
#include <boost/beast/http/string_body.hpp>
#include <exception>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <vector>

using json = nlohmann::json;

namespace routes {
void post_request(http::request<http::string_body>&& req,
                  tcp::socket& socket,
                  const std::shared_ptr<Storage>& storage) {
  try {
    auto data = json::parse(req.body());
    if (not data.is_array()) {
      throw std::runtime_error("Expected a JSON array");
    }
    std::vector<models::Entry> entries;

    for (const auto& item : data) {
      models::Entry entry;
      entry.time.hour = item.at("time").at("hour").get<int>();
      entry.time.min = item.at("time").at("min").get<double>();
      entry.value = item.at("value").get<int>();
      entries.push_back(entry);
    }

    storage->write_ahead_insert(entries);

    http::response<http::string_body> res{http::status::created, req.version()};
    res.set(http::field::content_type, "application/json");
    res.body() = "{\"status\":\"success\"}";
    res.prepare_payload();
    http::write(socket, res);
  } catch (const std::exception& e) {
    http::response<http::string_body> res{http::status::bad_request, req.version()};
    res.body() = std::string("{\"error\":\"Invalid JSON: ") + e.what() + "\"}";
    res.prepare_payload();
    http::write(socket, res);
  }
}
}  // namespace routes
