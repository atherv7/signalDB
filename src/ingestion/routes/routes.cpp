#include "routes.h"

#include <boost/beast/http/impl/write.hpp>
#include <boost/beast/http/string_body.hpp>
#include <exception>
#include <nlohmann/json.hpp>
#include <vector>

#include "ingestion/helpers/helpers.h"

using json = nlohmann::json;

namespace routes {
void post_request(http::request<http::string_body>&& req,
                  tcp::socket& socket,
                  const std::shared_ptr<Storage>& storage) {
  try {
    std::vector<models::Entry> entries = helpers::parse_json_for_entries(req.body());

    storage->write_ahead_insert(entries);
    storage->memtable_queue_insert(entries);

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
