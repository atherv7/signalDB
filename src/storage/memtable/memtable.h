#pragma once
#include "storage/models.h"
#include <string>
#include <vector>

class MemTable {
public:
  MemTable(int capacity, std::string &storage_file);

  /*
   * insert entry to memtable
   */
  void insert(models::Entry entry);

  /*
   * delete entry from memtable
   */
  auto delete_entry(models::Entry &entry) -> bool;

  /*
   * check if buffer contains entry
   */
  auto contains(models::Entry &entry) -> bool;

  /*
   * get reference of buffer
   */
  auto get_buffer() -> std::vector<models::Entry> &;

  /*
   *
   */
  void clear();

  friend std::ostream &operator<<(std::ostream &os, const MemTable &mem_table) {
    for (const auto &entry : mem_table.buffer) {
      os << entry << "\n";
    }

    return os;
  }

private:
  std::vector<models::Entry> buffer;
  std::string storage_file;
  int entry_to_write{0};
  int capacity;
  std::vector<models::Entry> write_to_file_queue;
};
