#pragma once

#include <mutex>
#include <string>
#include <vector>

#include "storage/models.h"

class FileStore {
 public:
  explicit FileStore(const std::string& storage_file);

  void write_entries(const std::vector<models::Entry>& entries);

  auto search_entries(std::function<bool(const models::Entry&)>& predicate)
      -> std::vector<models::Entry>;

 protected:
  std::string storage_file;
  mutable std::mutex file_mutex;
};
