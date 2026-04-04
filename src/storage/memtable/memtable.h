#pragma once

#include <mutex>
#include <vector>

#include "storage/models.h"

class MemTable {
 public:
  MemTable(int memtable_capacity, int max_level = 16, float probability = 0.5f);

  ~MemTable();

  /*
   * insert entries to queue for memtable
   */
  void insert_queue(const std::vector<models::Entry>& entries);

 private:
  struct MemTableNode {
    models::Entry entry;
    std::vector<MemTableNode*> next;

    MemTableNode(models::Entry entry, int level) : entry(entry), next(level + 1, nullptr) {}
  };

  std::vector<models::Entry>* queue;
  std::mutex lock;
  int memtable_capacity;
  int memtable_size;
  int max_level;
  float probability;
  int current_level;
  MemTableNode* memtable_head;

  /*
   * remove entries from queue to memtable
   */
  void from_queue_to_memtable();

  void insert_memtable(models::Entry entry);

  /*
   * flush memtable to file
   */
  void flush_memtable();

  [[nodiscard]] auto random_level() const -> int;
};
