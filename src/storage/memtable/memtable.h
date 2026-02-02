#pragma once

#include "storage/models.h"
#include <functional>
#include <vector>

class MemTable {
public:
  MemTable(int capacity, std::function<void(models::Entry)> &flush_queue);

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

  auto get_before(models::Timestamp &time) -> std::vector<models::Entry>;

  auto get_after(models::Timestamp &time) -> std::vector<models::Entry>;

  auto get_between(models::Timestamp &before_time,
                   models::Timestamp &after_time) -> std::vector<models::Entry>;

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
  std::function<void(models::Entry)> flush_queue;
  int entry_to_write{0};
  int capacity;
};
