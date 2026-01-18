#pragma once

#include "models.h"
#include <functional>
#include <iostream>
#include <ostream>

class Storage {
public:
  Storage(int capacity, std::string storage_file);

  /*
   * insert entry into storage
   */
  void insert(models::Entry entry);

  /*
   * get specific entry
   */
  bool has_entry(models::Entry &entry);

  /*
   * get entries before timestamp
   */
  std::vector<models::Entry> get_before(models::Timestamp &time);

  /*
   * get entries after timestamp
   */
  std::vector<models::Entry> get_after(models::Timestamp &time);

  /*
   * get entries in between timestamps
   */
  std::vector<models::Entry> get_between(models::Timestamp &before_time,
                                         models::Timestamp &after_time);

  /*
   * delete entry from storage
   */
  bool delete_entry(models::Entry &entry);

  std::string to_string();

  friend std::ostream &operator<<(std::ostream &os, const Storage &s) {
    for (const auto &entry : s.storage) {
      os << entry << "\n";
    }

    return os;
  }

private:
  std::vector<models::Entry> storage;
  int capacity;
  std::string storage_file;

  bool created_file;

  /*
   * flush in-memory storage to file
   */
  void flush();

  std::vector<models::Entry>
  search_in_file(std::function<bool(const models::Entry &)> comparison);

  bool delete_from_file(models::Entry &entry);

  bool check_from_file(models::Entry &entry);
};
