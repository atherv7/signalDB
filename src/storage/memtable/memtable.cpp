#include "memtable.h"

#include <functional>
#include <vector>

#include "storage/models.h"

MemTable::MemTable(int capacity, std::function<void(models::Entry)> flush_queue)
    : capacity{capacity}, buffer(capacity), flush_queue{std::move(flush_queue)} {}

void MemTable::insert(models::Entry entry) {
  if (this->buffer[this->entry_to_write] != models::Entry{}) {
    this->flush_queue(this->buffer[this->entry_to_write]);
  }

  this->buffer.at(this->entry_to_write) = entry;
  this->entry_to_write = (this->entry_to_write + 1) % this->capacity;
}

auto MemTable::delete_entry(models::Entry& entry) -> bool {
  std::vector<models::Entry> new_buffer(this->capacity);
  bool delete_occurred = false;
  for (models::Entry curr_entry : this->buffer) {
    if (curr_entry != entry) {
      new_buffer.push_back(curr_entry);
    } else {
      delete_occurred = true;
    }
  }
  this->buffer = new_buffer;

  return delete_occurred;
}

auto MemTable::contains(models::Entry& entry) -> bool {
  for (models::Entry& curr_entry : this->buffer) {
    if (curr_entry == entry) {
      return true;
    }
  }
  return false;
}

auto MemTable::get_before(models::Timestamp& time) -> std::vector<models::Entry> {
  std::vector<models::Entry> entries;
  for (const auto& entry : this->buffer) {
    if (entry.time < time) {
      entries.push_back(entry);
    }
  }
  return entries;
}

auto MemTable::get_after(models::Timestamp& time) -> std::vector<models::Entry> {
  std::vector<models::Entry> entries;
  for (const auto& entry : this->buffer) {
    if (entry.time > time) {
      entries.push_back(entry);
    }
  }

  return entries;
}

auto MemTable::get_between(models::Timestamp& before_time, models::Timestamp& after_time)
    -> std::vector<models::Entry> {
  std::vector<models::Entry> entries;
  for (const auto& entry : this->buffer) {
    if (entry.time > before_time && entry.time < after_time) {
      entries.push_back(entry);
    }
  }
  return entries;
}

auto MemTable::get_buffer() -> std::vector<models::Entry>& {
  return this->buffer;
}

void MemTable::clear() {
  this->buffer.clear();
  this->buffer.assign(this->capacity, models::Entry{});
}
