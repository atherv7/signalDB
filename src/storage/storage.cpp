#include "storage.h"

#include <vector>

#include "storage/memtable/memtable.h"

// TODO: smart pointers?
// TODO: adjust memtable capacity
Storage::Storage(const std::string& storage_file) {
  this->write_ahead = new FileStore(storage_file + ".ahead");
  this->memtable = new MemTable(20);
}

void Storage::write_ahead_insert(const std::vector<models::Entry>& entries) {
  this->write_ahead->write_entries(entries);
}

void Storage::memtable_queue_insert(const std::vector<models::Entry>& entries) {
  this->memtable->insert(entries);
}
