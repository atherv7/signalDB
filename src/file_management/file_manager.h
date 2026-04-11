#pragma once

#include <string>
#include <vector>

#include "file_management/file_store/file_store.h"
#include "storage/models.h"

class FileManager {
 public:
  FileManager(const std::string& storage_file_name);

  void add_to_file(const std::vector<models::Timestamp>& delta_delta_time,
                   const std::vector<int>& delta_values);

 private:
  FileStore* file;
};
