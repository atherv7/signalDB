#include "storage.h"
#include "memtable/memtable.h"
#include "models.h"
#include "storage/file_management/file_management.h"
#include <functional>
#include <future>
#include <sstream>
#include <vector>

Storage::Storage(int capacity, int file_queue_cap, std::string storage_file)
    : storage_file(storage_file) {
  this->file_management = new FileManagement(storage_file, file_queue_cap);
  this->mem_store = new MemTable(capacity, [&](models::Entry entry) {
    this->file_management->insert_flush_queue(entry);
  });
}

void Storage::insert(models::Entry entry) { this->mem_store->insert(entry); }

auto Storage::has_entry(models::Entry &entry) -> bool {
  std::future<std::vector<models::Entry>> fut =
      this->file_management->has_in_file(entry);

  bool contains = this->mem_store->contains(entry);
  if (contains) {
    return true;
  }

  bool result = fut.get().size() > 0;
  return result;
}

auto Storage::get_before(models::Timestamp &time)
    -> std::vector<models::Entry> {
  std::future<std::vector<models::Entry>> fut =
      this->file_management->get_before(time);
  // TODO: implement get_before for mem_table
  std::vector<models::Entry> entries = fut.get();

  return entries;
}

std::vector<models::Entry> Storage::get_after(models::Timestamp &time) {
  std::future<std::vector<models::Entry>> fut =
      this->file_management->get_after(time);

  std::vector<models::Entry> entries = fut.get();

  return entries;
}

auto Storage::get_between(models::Timestamp &before_time,
                          models::Timestamp &after_time)
    -> std::vector<models::Entry> {
  std::future<std::vector<models::Entry>> fut =
      this->file_management->get_between(before_time, after_time);

  std::vector<models::Entry> entries = fut.get();

  return entries;
}

auto Storage::to_string() -> std::string {
  std::ostringstream oss;
  oss << *this->mem_store;
  return oss.str();
}
