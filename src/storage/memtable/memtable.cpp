#include "memtable.h"
#include "storage/models.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

MemTable::MemTable(int capacity, std::string storage_file, int file_queue_cap)
    : capacity{capacity}, storage_file{storage_file}, entry_to_write{0},
      buffer(capacity), file_queue_cap(file_queue_cap) {}

void MemTable::insert(models::Entry entry) {
  // TODO: current implementation the flushing is handled synchronously
  // change so it is handled asynchronously
  if (this->buffer[this->entry_to_write] != models::Entry{}) {
    this->write_to_file_queue.push_back(this->buffer[this->entry_to_write]);
    if (this->write_to_file_queue.size() == this->file_queue_cap) {
      this->flush_to_file();
    }
  }
  this->buffer[this->entry_to_write] = entry;
  this->entry_to_write = (this->entry_to_write + 1) % this->capacity;
}

bool MemTable::delete_entry(models::Entry &entry) {
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

bool MemTable::contains(models::Entry &entry) {
  if (this->contains_file) {
    // TODO: start another thread for file search to prevent
    // buffer search from being blocked
    this->search_in_file(entry);
  }
  for (models::Entry &curr_entry : this->buffer) {
    if (curr_entry == entry) {
      return true;
    }
  }

  return false;
}

std::vector<models::Entry> &MemTable::get_buffer() { return this->buffer; }

void MemTable::clear() { this->buffer.clear(); }

void MemTable::write_to_file(std::vector<models::Entry> &entries) {
  std::ofstream output_file(this->storage_file,
                            std::ios::binary | std::ios::out | std::ios::trunc);
  if (!output_file.is_open()) {
    std::cerr << "Error: unable to open file for writing\n";
    return;
  }

  int size_of_entry = sizeof(models::Entry);

  for (const auto &entry : entries) {
    output_file.write(reinterpret_cast<const char *>(&entry), size_of_entry);
  }
  output_file.close();

  this->clear();

  this->contains_file = true;
}

void MemTable::flush_to_file() { this->write_to_file(this->buffer); }

bool MemTable::search_in_file(models::Entry &entry) {
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

  this->write_to_file(file_entries);

  return delete_occurred;
}
