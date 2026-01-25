#pragma once

#include "memtable/memtable.h"
#include "models.h"
#include <functional>
#include <iostream>
#include <ostream>

class Storage {
public:
  Storage(int capacity, int file_queue_cap, std::string storage_file);

  /*
   * insert entry into storage
   */
  void insert(models::Entry entry);

  /*
   * get specific entry
   */
  auto has_entry(models::Entry &entry) -> bool;

  /*
   * get entries before timestamp
   */
  auto get_before(models::Timestamp &time) -> std::vector<models::Entry>;

  /*
   * get entries after timestamp
   */
  auto get_after(models::Timestamp &time) -> std::vector<models::Entry>;

  /*
   * get entries in between timestamps
   */
  auto get_between(models::Timestamp &before_time,
                   models::Timestamp &after_time) -> std::vector<models::Entry>;

  /*
   * delete entry from storage
   */
  auto delete_entry(models::Entry &entry) -> bool;

  std::string to_string();

  friend std::ostream &operator<<(std::ostream &os, const Storage &s) {
    os << s.mem_store << "\n";
    return os;
  }

private:
  MemTable *mem_store;
  std::string storage_file;
  bool created_file{false};

  /*
   * search in file for entry
   */
  std::vector<models::Entry>
  search_in_file(std::function<bool(const models::Entry &)> &comparison);

  /*
   * delete entry from file, if present
   */
  bool delete_from_file(models::Entry &entry);

  /*
   * check if entry is present in file
   */
  bool check_from_file(models::Entry &entry);
};
