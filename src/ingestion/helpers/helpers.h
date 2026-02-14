#pragma once

#include <nlohmann/json.hpp>

#include "storage/models.h"

using json = nlohmann::json;

namespace helpers {
std::vector<models::Entry> parse_json_for_entries(std::string& json_string);
};
