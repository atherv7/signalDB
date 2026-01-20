#include "storage.h"
#include "memtable/memtable.h"
#include "models.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

Storage::Storage(int capacity, std::string storage_file)
    : storage_file(storage_file) {
  this->mem_store = new MemTable(capacity, storage_file, 2);
}

auto Storage::delete_from_file(models::Entry &entry) -> bool {
  if (!created_file) {
    return false;
  }

  std::ifstream input_file(storage_file, std::ios::binary | std::ios::in);
  if (!input_file.is_open()) {
    std::cerr << "Error opening file for reading\n";
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
  std::remove(storage_file.c_str());

  std::ofstream output_file(storage_file,
                            std::ios::binary | std::ios::out | std::ios::trunc);
  if (!output_file.is_open()) {
    std::cerr << "Error opening file for writing\n";
    return false;
  }

  for (const auto &entry : file_entries) {
    output_file.write(reinterpret_cast<const char *>(&entry), size_of_entry);
  }

  output_file.close();

  return delete_occurred;
}

auto Storage::check_from_file(models::Entry &entry) -> bool {
  if (!created_file) {
    return false;
  }

  std::ifstream input_file(storage_file, std::ios::binary | std::ios::in);
  if (!input_file.is_open()) {
    std::cerr << "Error opening file for reading\n";
    return false;
  }

  int size_of_entry = sizeof(models::Entry);
  models::Entry current_entry;

  bool delete_occurred = false;

  while (input_file.read(reinterpret_cast<char *>(&current_entry),
                         size_of_entry)) {
    if (current_entry == entry) {
      return true;
    }
  }

  input_file.close();

  return false;
}

void Storage::insert(models::Entry entry) { this->mem_store->insert(entry); }

auto Storage::has_entry(models::Entry &entry) -> bool {
  return this->mem_store->contains(entry);
}

auto Storage::get_before(models::Timestamp &time)
    -> std::vector<models::Entry> {
  std::function<bool(const models::Entry &)> search_func =
      [&time](const models::Entry &entry) { return entry.time < time; };
  std::vector<models::Entry> entries{this->search_in_file(search_func)};

  for (auto &entry : this->mem_store->get_buffer()) {
    if (entry.time < time) {
      entries.push_back(entry.clone());
    }
  }

  return entries;
}

std::vector<models::Entry> Storage::get_after(models::Timestamp &time) {
  std::function<bool(const models::Entry &)> search_func =
      [&time](const models::Entry &entry) { return entry.time > time; };
  std::vector<models::Entry> entries{this->search_in_file(search_func)};

  for (auto &entry : this->mem_store->get_buffer()) {
    if (entry.time > time) {
      entries.push_back(entry.clone());
    }
  }

  return entries;
}

auto Storage::get_between(models::Timestamp &before_time,
                          models::Timestamp &after_time)
    -> std::vector<models::Entry> {
  std::function<bool(const models::Entry &)> search_func =
      [&before_time, &after_time](const models::Entry &entry) {
        return entry.time > before_time && entry.time < after_time;
      };
  std::vector<models::Entry> entries{this->search_in_file(search_func)};

  for (auto &entry : this->mem_store->get_buffer()) {
    if (entry.time > before_time && entry.time < after_time) {
      entries.push_back(entry.clone());
    }
  }

  return entries;
}

auto Storage::delete_entry(models::Entry &entry) -> bool {
  if (!this->mem_store->delete_entry(entry)) {
    return this->delete_from_file(entry);
  }

  return false;
}

auto Storage::to_string() -> std::string {
  std::ostringstream oss;
  oss << *this->mem_store;
  return oss.str();
}
