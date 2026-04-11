#include "file_store.h"

#include <fstream>

FileStore::FileStore(const std::string& storage_file) : storage_file(storage_file) {}

void FileStore::write_sync(const std::vector<models::Entry>& entries) {
  if (entries.empty()) {
    return;
  }
  int size_of_entry = sizeof(models::Entry);

  std::ofstream out(this->storage_file, std::ios::binary | std::ios::app);
  if (not out.is_open()) {
    std::cerr << "Failed to open file: " << this->storage_file << "\n";
    return;
  }

  // TODO: need to serialize entry before inserting into WAL
  for (const auto& entry : entries) {
    out.write(reinterpret_cast<const char*>(&entry), size_of_entry);
  }
  out.close();
}

void FileStore::write_async(const void* data, size_t size) {
  off_t offset = this->write_offset.fetch_add(size);
  pwrite(fd, data, size, offset);
}

void FileStore::flush() const {
  fdatasync(fd);
}
