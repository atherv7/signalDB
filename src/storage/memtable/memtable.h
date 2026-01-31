#pragma once

#include "storage/models.h"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>

class MemTable {
public:
  MemTable(int capacity, std::string &storage_file, int file_queue_cap);

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

  auto search_in_file(std::function<bool(const models::Entry &)> &comparison)
      -> std::vector<models::Entry>;

private:
  std::vector<models::Entry> buffer;
  std::string storage_file;
  int entry_to_write{0};
  int capacity;
  std::vector<models::Entry> write_to_file_queue;
  std::mutex file_queue_mutex;
  std::jthread file_flush_thread;
  int file_queue_cap;
  bool contains_file{false};
  std::condition_variable_any flush_cond_var;
  std::jthread file_search_thread;
  std::mutex file_search_mutex;
  std::vector<models::Task> file_search_queue;
  std::condition_variable_any search_cond_var;

  /*
   * flush entries to file
   */
  void flush_to_file(std::stop_token stoken);

  void write_to_file(std::vector<models::Entry> &entries,
                     std::ofstream &output_file);

  bool has_in_file(models::Entry &entry);

  bool delete_from_file(models::Entry &entry);

  void file_searching(std::stop_token stoken);
};
