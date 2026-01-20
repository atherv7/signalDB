#pragma once
#include "storage/models.h"
#include <string>
#include <vector>

class MemTable {
public:
  MemTable(int capacity, std::string storage_file, int file_queue_cap);

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
   *  clear the buffer
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
  int file_queue_cap;
  bool contains_file = false;

  /*
   * flush entries to file
   */
  void flush_to_file();

  void write_to_file(std::vector<models::Entry> &entries);

  bool search_in_file(models::Entry &entry);

  bool delete_from_file(models::Entry &entry);
};
