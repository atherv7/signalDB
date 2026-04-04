#include "storage.h"

#include <vector>

Storage::Storage(const std::string& storage_file) {
  this->write_ahead = new FileStore(storage_file + ".ahead");  // TODO: smart pointer?
}

void Storage::write_ahead_insert(const std::vector<models::Entry>& entries) {
  this->write_ahead->write_entries(entries);
}

void Storage::memtable_queue_insert(const std::vector<models::Entry>& entries) {}
