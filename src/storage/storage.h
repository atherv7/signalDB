#pragma once

#include <vector>

#include "file_management/file_store/file_store.h"
#include "models.h"
#include "storage/memtable/memtable.h"

class Storage {
 public:
  Storage(const std::string& storage_file);

  /*
   * write ahead log for data durability
   */
  void write_ahead_insert(const std::vector<models::Entry>& entries);

  /*
   * insert into memtable async
   */
  void memtable_queue_insert(const std::vector<models::Entry>& entries);

 private:
  FileStore* write_ahead;
  MemTable* memtable;
};
