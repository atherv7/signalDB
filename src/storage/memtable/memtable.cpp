#include "memtable.h"
#include "storage/models.h"
#include <iostream>
#include <sstream>
#include <vector>

MemTable::MemTable(int capacity, std::string &storage_file)
    : capacity{capacity}, storage_file{storage_file}, buffer(capacity) {}

void MemTable::insert(models::Entry entry) {
  if (this->buffer[this->entry_to_write] != models::Entry{}) {
    std::cout << "not empty, need to flush\n";
  }
  this->buffer[this->entry_to_write] = entry;
  this->entry_to_write = (this->entry_to_write + 1) % this->capacity;
}

auto MemTable::delete_entry(models::Entry &entry) -> bool {
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

  this->entry_to_write--;

  return delete_occurred;
}

auto MemTable::contains(models::Entry &entry) -> bool {
  for (models::Entry &curr_entry : this->buffer) {
    if (curr_entry == entry) {
      return true;
    }
  }

  return false;
}

auto MemTable::get_buffer() -> std::vector<models::Entry> & {
  return this->buffer;
}

void MemTable::clear() { this->buffer.clear(); }
