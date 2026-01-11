#include "storage.hpp"
#include <vector>

Storage::Storage(int capacity) : capacity(capacity) { this->storage = {}; }

void Storage::flush() {}

void Storage::insert(Entry entry) { this->storage.push_back(entry); }

std::vector<Entry *> Storage::get_before(Timestamp &time) {
  std::vector<Entry *> entries{};

  for (Entry entry : this->storage) {
    if (entry.time < time) {
      entries.push_back(&entry);
    }
  }

  return entries;
}

std::vector<Entry *> Storage::get_after(Timestamp &time) {
  std::vector<Entry *> entries{};

  for (Entry entry : this->storage) {
    if (entry.time > time) {
      entries.push_back(&entry);
    }
  }

  return entries;
}

std::vector<Entry *> Storage::get_between(Timestamp &before_time,
                                          Timestamp &after_time) {
  std::vector<Entry *> entries{};

  for (Entry entry : this->storage) {
    if (entry.time > before_time && entry.time < after_time) {
      entries.push_back(&entry);
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

  if (index_to_delete == -1) {
    return false;
  }

  this->storage.erase(this->storage.begin() + index_to_delete);
  return true;
}
