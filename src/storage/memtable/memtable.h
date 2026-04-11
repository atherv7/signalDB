#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <stop_token>
#include <thread>
#include <vector>

#include "file_management/file_manager.h"
#include "storage/models.h"

class MemTable {
 public:
  MemTable(int memtable_capacity,
           FileManager& file_manager,
           int max_level = 16,
           float probability = 0.5f);

  ~MemTable() = default;

  // prevent system crash from mutex, thread, and atomic copying
  MemTable(const MemTable&) = delete;
  MemTable& operator=(const MemTable&) = delete;

  /*
   * insert entries to queue for memtable
   */
  void insert(const std::vector<models::Entry>& entries);

  /*
   * start memtable instance threads
   */
  void start();

  /*
   * stop memtable instance threads
   */
  void stop();

 private:
  struct MemTableNode {
    models::Entry entry;
    std::unique_ptr<MemTableNode> next_owned;
    std::vector<MemTableNode*> levels;

    MemTableNode(models::Entry e, int level) : entry(e), levels(level + 1, nullptr) {}
  };

  FileManager& file_manager;

  std::vector<models::Entry> queue;
  std::mutex queue_lock;
  std::mutex memtable_lock;
  int memtable_capacity;        // TODO: transfer in terms of bytes
  unsigned long memtable_size;  // TODO: change to size in terms of bytes
  int max_level;
  float probability;
  int current_level;
  std::unique_ptr<MemTableNode> memtable_head;
  std::jthread queue_to_memtable_thread;
  std::condition_variable queue_to_memtable_cv;
  std::jthread memtable_to_file_thread;

  /*
   * remove entries from queue to memtable
   */
  void from_queue_to_memtable(std::stop_token stoken);

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
