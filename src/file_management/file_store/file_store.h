#pragma once

/*
 * file storage interface
 */

#include <mutex>
#include <string>
#include <vector>

#include "storage/models.h"

class FileStore {
 public:
  explicit FileStore(const std::string& storage_file);

  void write_entries(const std::vector<models::Entry>& entries);

 protected:
  std::string storage_file;
  mutable std::mutex file_mutex;
};
