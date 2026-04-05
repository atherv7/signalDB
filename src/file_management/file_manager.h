#pragma once

#include <string>

#include "file_management/file_store/file_store.h"

class FileManager {
 public:
  FileManager(const std::string& storage_file_name);

 private:
  FileStore* file;
};
