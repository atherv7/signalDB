#include "file_store.h"

#include <fstream>

FileStore::FileStore(const std::string& storage_file) : storage_file(storage_file) {}

void FileStore::write_entries(const std::vector<models::Entry>& entries) {
  if (entries.empty()) {
    return;
  }
  int size_of_entry = sizeof(models::Entry);

  std::lock_guard<std::mutex> lock(this->file_mutex);
  std::ofstream out(this->storage_file, std::ios::binary | std::ios::app);
  if (not out.is_open()) {
    std::cerr << "Failed to open file: " << this->storage_file << "\n";
    return;
  }

  for (const auto& entry : entries) {
    out.write(reinterpret_cast<const char*>(&entry), size_of_entry);
  }
  out.close();
}

auto FileStore::search_entries(std::function<bool(const models::Entry&)>& predicate)
    -> std::vector<models::Entry> {
  std::vector<models::Entry> results;
  int size_of_entry = sizeof(models::Entry);

  std::lock_guard<std::mutex> lock(this->file_mutex);
  std::ifstream in(this->storage_file, std::ios::binary);
  if (not in.is_open()) {
    std::cerr << "Failed to open file: " << this->storage_file << "\n";
    return results;
  }

  models::Entry entry;
  while (in.read(reinterpret_cast<char*>(&entry), size_of_entry)) {
    if (predicate(entry)) {
      results.push_back(entry);
    }
  }
  in.close();

  return results;
}
