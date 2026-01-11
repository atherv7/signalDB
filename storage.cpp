#include "storage.hpp"
#include <fstream>
#include <iostream>
#include <vector>

Storage::Storage(int capacity) : capacity(capacity) { this->storage = {}; }

void Storage::flush() {
  std::ofstream output_file("file_storage.txt",
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
}

std::vector<Entry>
Storage::search_in_file(std::function<bool(const Entry &)> comparison) {
  std::vector<Entry> entries{};

  std::ifstream input_file("file_storage.txt", std::ios::binary | std::ios::in);
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
  std::ifstream input_file("file_storage.txt", std::ios::binary | std::ios::in);
  if (!input_file.is_open()) {
    std::cerr << "Error opening file for reading" << std::endl;
    return false;
  }

  int size_of_entry = sizeof(Entry);
  Entry current_entry;
  std::vector<Entry> file_entries{};

  while (input_file.read(reinterpret_cast<char *>(&current_entry),
                         size_of_entry)) {
    file_entries.push_back(current_entry);
  }

  input_file.close();
  int ind_to_delete{-1};
  for (int ind = 0; ind < file_entries.size(); ind++) {
    if (file_entries[ind] == entry) {
      ind_to_delete = ind;
      break;
    }
  }

  bool outcome = false;

  if (ind_to_delete != -1) {
    file_entries.erase(file_entries.begin() + ind_to_delete);
    outcome = true;
  }

  std::remove("file_storage.txt");

  std::ofstream output_file("file_storage.txt",
                            std::ios::binary | std::ios::out | std::ios::trunc);
  if (!output_file.is_open()) {
    std::cerr << "Error opening file for writing" << std::endl;
    return false;
  }

  for (const auto &entry : file_entries) {
    output_file.write(reinterpret_cast<const char *>(&entry), size_of_entry);
  }

  output_file.close();

  return outcome;
}

void Storage::insert(Entry entry) {
  if (this->storage.size() == this->capacity) {
    flush();
  }
  this->storage.push_back(entry);
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
}

void Storage::print_storage() {
  for (Entry &entry : this->storage) {
    std::cout << entry << std::endl;
  }
}
