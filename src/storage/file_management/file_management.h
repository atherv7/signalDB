#pragma once

#include "storage/models.h"
#include <condition_variable>
#include <fstream>
#include <future>
#include <iterator>
#include <mutex>
#include <queue>
#include <stop_token>
#include <thread>
#include <utility>
#include <vector>

// TODO: when delete entry is implemented
// and the file is deleted and then rewritten
// may require the "renewing" of the ofstream and
// ifstream references the threads have
class FileManagement {
public:
  FileManagement(std::string &storage_file, int file_flush_cap);
  ~FileManagement();

  auto has_file() -> const bool;

  void insert_flush_queue(models::Entry entry);

  auto has_in_file(models::Entry &entry) -> bool;

private:
  std::string storage_file;
  bool contains_file{false};
  std::mutex file_mutex;

  // file flushing
  std::jthread file_flush_worker_;
  std::vector<models::Entry> file_flush_queue;
  std::mutex flush_queue_mutex;
  int flush_queue_cap;
  std::condition_variable_any flush_cond_var;

  // file searching
  std::jthread file_search_worker_;
  std::mutex file_search_mutex;
  std::vector<std::pair<models::Task, std::promise<std::vector<models::Entry>>>>
      file_search_queue;
  std::condition_variable_any search_cond_var;

  void flush_to_file(std::stop_token stoken);

  void write_to_file(std::vector<models::Entry> &entries,
                     std::ofstream &output_file);

  void file_search(std::stop_token stoken);

  auto search_in_file(std::function<bool(const models::Entry &)> &comparison,
                      std::ifstream &input_file) -> std::vector<models::Entry>;
};
