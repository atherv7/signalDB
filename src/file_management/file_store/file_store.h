#pragma once

/*
 * file storage interface
 */

#include <atomic>
#include <string>
#include <vector>

#include "storage/models.h"

class FileStore {
 public:
  explicit FileStore(const std::string& storage_file);

  void write_sync(const std::vector<models::Entry>& entries);

  void write_async(const void* data, size_t size);

  void flush() const;

 protected:
  std::string storage_file;
  int fd;
  std::atomic<off_t> write_offset;
};
