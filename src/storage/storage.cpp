#include "storage.h"

Storage::Storage(const std::string& storage_file, FileManager& file_manager) {
  this->write_ahead = std::make_unique<FileStore>(storage_file + ".ahead");
  this->memtable = std::make_unique<MemTable>(20 * sizeof(models::Entry), file_manager);
}

void Storage::write_ahead_insert(const std::vector<models::Entry>& entries) {
  this->write_ahead->write_sync(entries);
}

void Storage::memtable_queue_insert(const std::vector<models::Entry>& entries) {
  this->memtable->insert(entries);
}
