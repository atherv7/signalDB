#include "memtable.h"
#include "storage/file_management/file_management.h"
#include "storage/models.h"
#include <cinttypes>
#include <fstream>
#include <iostream>
#include <iterator>
#include <mutex>
#include <sstream>
#include <stop_token>
#include <thread>
#include <vector>

// TODO: remove file_queue_cap class member
MemTable::MemTable(int capacity, std::string &storage_file, int file_queue_cap)
    : capacity{capacity}, buffer(capacity) {
  this->file_manage = new FileManagement(storage_file, file_queue_cap);
}

void MemTable::insert(models::Entry entry) {
  if (this->buffer[this->entry_to_write] != models::Entry{}) {
    this->file_manage->insert_flush_queue(this->buffer[this->entry_to_write]);
  }

  this->buffer.at(this->entry_to_write) = entry;
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

  if (!delete_occurred) {
    delete_occurred = this->delete_from_file(entry);
  } else {
    this->entry_to_write--;
  }

  return delete_occurred;
}

auto MemTable::contains(models::Entry &entry) -> bool {
  if (this->contains_file) {
    // TODO: start another thread for file search to prevent
    // buffer search from being blocked
    return this->has_in_file(entry);
  }
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

void MemTable::clear() {
  this->buffer.clear();
  this->buffer.assign(this->capacity, models::Entry{});
}

bool MemTable::has_in_file(models::Entry &entry) {
  std::ifstream input_file(this->storage_file, std::ios::binary | std::ios::in);
  if (!input_file.is_open()) {
    std::cerr << "Error: unable to open file for reading\n";
    return false;
  }

  int size_of_entry = sizeof(models::Entry);
  models::Entry current_entry;

  while (input_file.read(reinterpret_cast<char *>(&current_entry),
                         size_of_entry)) {
    if (current_entry == entry) {
      return true;
    }
  }
  input_file.close();

  return false;
}

bool MemTable::delete_from_file(models::Entry &entry) {
  std::ifstream input_file(this->storage_file, std::ios::binary | std::ios::in);
  if (!input_file.is_open()) {
    std::cerr << "Error: unable to open file for reading\n";
    return false;
  }

  int size_of_entry = sizeof(models::Entry);
  models::Entry current_entry;
  std::vector<models::Entry> file_entries{};

  bool delete_occurred = false;

  while (input_file.read(reinterpret_cast<char *>(&current_entry),
                         size_of_entry)) {
    if (current_entry != entry) {
      file_entries.push_back(current_entry);
    } else {
      delete_occurred = true;
    }
  }
  input_file.close();
  std::remove(this->storage_file.c_str());

  std::ofstream output_file(this->storage_file,
                            std::ios::binary | std::ios::out | std::ios::trunc);
  this->write_to_file(file_entries, output_file);

  return delete_occurred;
}
