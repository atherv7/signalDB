#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "storage/models.h"

class MemTable {
 public:
  // TODO: rule of six
  MemTable(int memtable_capacity, int max_level = 16, float probability = 0.5f);

  ~MemTable();

  /*
   * insert entries to queue for memtable
   */
  void insert(const std::vector<models::Entry>& entries);

  void start();

  void stop();

 private:
  struct MemTableNode {
    models::Entry entry;
    std::vector<MemTableNode*> next;

    MemTableNode(models::Entry entry, int level) : entry(entry), next(level + 1, nullptr) {}
  };

  std::vector<models::Entry>* queue;
  std::mutex queue_lock;
  std::mutex memtable_lock;
  int memtable_capacity;
  unsigned long memtable_size;  // TODO: change to size in terms of bytes
  int max_level;
  float probability;
  int current_level;
  MemTableNode* memtable_head;

  std::atomic<bool> memtable_running;

  std::thread queue_to_memtable_thread;
  std::condition_variable queue_to_memtable_cv;

  std::thread memtable_to_file_thread;

  /*
   * remove entries from queue to memtable
   */
  void from_queue_to_memtable();

  /*
   * insert entry into memtable
   */
  void insert_memtable(models::Entry entry);

  /*
   * flush memtable to file
   */
  void flush_memtable();

  /*
   * get random level for skip list node
   */
  [[nodiscard]] auto random_level() const -> int;
};
