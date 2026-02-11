#include "helpers.h"

#include <exception>
#include <vector>

#include "storage/models.h"

namespace helpers {
std::vector<models::Entry> parse_json_for_entries(std::string& json_string) {
  std::vector<models::Entry> entries;

  try {
    auto data = json::parse(json_string);
    if (not data.is_array()) {
      throw std::runtime_error("Expected a JSON array");
    }

    for (const auto& item : data) {
      models::Entry entry;
      entry.time.hour = item.at("time").at("hour").get<int>();
      entry.time.min = item.at("time").at("min").get<double>();
      entry.value = item.at("value").get<int>();
      entries.push_back(entry);
    }
  } catch (const std::exception& e) {
    std::cerr << "Failed to parse json string\n";
  }

  return entries;
}
}  // namespace helpers
