#pragma once

#include <iostream>
#include <ostream>

#include "memtable/memtable.h"
#include "models.h"
#include "storage/file_management/file_management.h"

class Storage {
 public:
  Storage(int capacity, std::string storage_file, int file_queue_cap);

  /*
   * insert entry into storage
   */
  void insert(models::Entry entry);

  /*
   * get specific entry
   */
  auto has_entry(models::Entry& entry) -> bool;

  /*
   * get entries before timestamp
   */
  auto get_before(models::Timestamp& time) -> std::vector<models::Entry>;

  /*
   * get entries after timestamp
   */
  auto get_after(models::Timestamp& time) -> std::vector<models::Entry>;

  /*
   * get entries in between timestamps
   */
  auto get_between(models::Timestamp& before_time, models::Timestamp& after_time)
      -> std::vector<models::Entry>;

  /*
   * delete entry from storage
   */
  auto delete_entry(models::Entry& entry) -> bool;

  std::string to_string();

  friend std::ostream& operator<<(std::ostream& os, const Storage& s) {
    os << s.mem_store << "\n";
    return os;
  }

 private:
  MemTable* mem_store;
  FileManagement* file_management;
  std::string storage_file;
  bool created_file{false};

  /*
   * delete entry from file, if present
   */
  bool delete_from_file(models::Entry& entry);
};
