#include "storage.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

Storage::Storage(int capacity, std::string storage_file)
    : capacity(capacity), storage_file(storage_file), created_file(false) {
  this->storage = {};
}

void Storage::flush() {
  std::ofstream output_file(storage_file,
                            std::ios::binary | std::ios::out | std::ios::trunc);
  if (!output_file.is_open()) {
    std::cerr << "Error opening file for writing" << std::endl;
    return;
  }

  int size_of_entry = sizeof(Entry);

  for (const auto &entry : this->storage) {
    output_file.write(reinterpret_cast<const char *>(&entry), size_of_entry);
  }

  output_file.close();
  this->storage.clear();

  created_file = true;
}

std::vector<Entry>
Storage::search_in_file(std::function<bool(const Entry &)> comparison) {
  std::vector<Entry> entries{};

  if (!created_file) {
    return entries;
  }

  std::ifstream input_file(storage_file, std::ios::binary | std::ios::in);
  if (!input_file.is_open()) {
    std::cerr << "Error opening file for reading" << std::endl;
    return entries;
  }

  int size_of_entry = sizeof(Entry);
  Entry current_entry;
  std::vector<Entry> saved_entries{};

  while (input_file.read(reinterpret_cast<char *>(&current_entry),
                         size_of_entry)) {
    saved_entries.push_back(current_entry);
  }

  input_file.close();

  for (auto entry : saved_entries) {
    if (comparison(entry)) {
      entries.push_back(entry);
    }
  }

  return entries;
}

bool Storage::delete_from_file(Entry &entry) {
  if (!created_file) {
    return false;
  }

  std::ifstream input_file(storage_file, std::ios::binary | std::ios::in);
  if (!input_file.is_open()) {
    std::cerr << "Error opening file for reading" << std::endl;
    return false;
  }

  int size_of_entry = sizeof(Entry);
  Entry current_entry;
  std::vector<Entry> file_entries{};

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
    std::cerr << "Error opening file for writing" << std::endl;
    return false;
  }

  for (const auto &entry : file_entries) {
    output_file.write(reinterpret_cast<const char *>(&entry), size_of_entry);
  }

  output_file.close();

  return delete_occurred;
}

bool Storage::check_from_file(Entry &entry) {
  if (!created_file) {
    return false;
  }

  std::ifstream input_file(storage_file, std::ios::binary | std::ios::in);
  if (!input_file.is_open()) {
    std::cerr << "Error opening file for reading" << std::endl;
    return false;
  }

  int size_of_entry = sizeof(Entry);
  Entry current_entry;

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

void Storage::insert(Entry entry) {
  if (this->storage.size() == this->capacity) {
    flush();
  }
  this->storage.push_back(entry);
}

bool Storage::has_entry(Entry &entry) {
  for (Entry &e : this->storage) {
    if (e == entry)
      return true;
  }

  return this->check_from_file(entry);
}

std::vector<Entry> Storage::get_before(Timestamp &time) {
  std::vector<Entry> entries{this->search_in_file(
      [&time](const Entry &entry) { return entry.time < time; })};

  for (auto &entry : this->storage) {
    if (entry.time < time) {
      entries.push_back(entry.clone());
    }
  }

  return entries;
}

std::vector<Entry> Storage::get_after(Timestamp &time) {
  std::vector<Entry> entries{this->search_in_file(
      [&time](const Entry &entry) { return entry.time > time; })};

  for (auto &entry : this->storage) {
    if (entry.time > time) {
      entries.push_back(entry.clone());
    }
  }

  return entries;
}

std::vector<Entry> Storage::get_between(Timestamp &before_time,
                                        Timestamp &after_time) {
  std::vector<Entry> entries{
      this->search_in_file([&before_time, &after_time](const Entry &entry) {
        return entry.time > before_time && entry.time < after_time;
      })};

  for (auto &entry : this->storage) {
    if (entry.time > before_time && entry.time < after_time) {
      entries.push_back(entry.clone());
    }
  }

  return entries;
}

bool Storage::delete_entry(Entry &entry) {
  int before_size = this->storage.size();

  int index_to_delete = -1;

  for (int ind = 0; ind < this->storage.size(); ind++) {
    if (this->storage[ind] == entry) {
      index_to_delete = ind;
      break;
    }
  }

  if (index_to_delete != -1) {
    this->storage.erase(this->storage.begin() + index_to_delete);
    return true;
  } else {
    return this->delete_from_file(entry);
  }

  return false;
}

std::string Storage::to_string() {
  std::ostringstream oss;

  if (created_file) {
    std::ifstream input_file(storage_file, std::ios::binary | std::ios::in);
    if (!input_file.is_open()) {
      std::cerr << "Error opening file for reading" << std::endl;
      return "";
    }

    int size_of_entry = sizeof(Entry);
    Entry current_entry;
    while (input_file.read(reinterpret_cast<char *>(&current_entry),
                           size_of_entry)) {
      oss << current_entry << "\n";
    }
    input_file.close();
  }

  for (const auto &entry : this->storage) {
    oss << entry << "\n";
  }

  return oss.str();
}
