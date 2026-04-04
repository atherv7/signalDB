#pragma once

#include <vector>

#include "models.h"
#include "storage/file_management/file_store.h"

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
};
