#include "file_store.h"

#include <unistd.h>

#include <cerrno>
#include <fstream>
#include <nlohmann/json_fwd.hpp>

FileStore::FileStore(const std::string& storage_file) : storage_file{storage_file}, fd{-1} {}

void FileStore::write_sync(const std::vector<models::Entry>& entries) {
  if (entries.empty()) {
    return;
  }

  std::ofstream out(this->storage_file, std::ios::binary | std::ios::app);

  for (const auto& entry : entries) {
    nlohmann::json j = entry;
    std::vector<uint8_t> v = nlohmann::json::to_cbor(j);

    uint32_t size = static_cast<uint32_t>(v.size());
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    out.write(reinterpret_cast<const char*>(v.data()), size);
  }
}

void FileStore::write_async(const void* data, size_t size) {
  off_t current_offset = this->write_offset.fetch_add(static_cast<off_t>(size));
  const char* ptr = static_cast<const char*>(data);
  size_t remaining = size;

  while (remaining > 0) {
    ssize_t result = pwrite(this->fd, ptr, remaining, current_offset);

    if (result == -1) {
      if (errno == EINTR) {
        continue;
      }
      return;
    }

    ptr += result;
    remaining -= result;
    current_offset += result;
  }
}

void FileStore::flush() const {
  fdatasync(this->fd);
}
