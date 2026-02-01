#include "file_management.h"
#include "storage/models.h"
#include <fstream>
#include <future>
#include <ios>
#include <iostream>
#include <mutex>
#include <stop_token>
#include <utility>
#include <vector>

auto FileManagement::search_in_file(
    std::function<bool(const models::Entry &)> &comparison,
    std::ifstream &input_file) -> std::vector<models::Entry> {
  std::vector<models::Entry> entries{};

  if (!this->contains_file) {
    return entries;
  }

  input_file.open(this->storage_file, std::ios::binary | std::ios::in);
  if (!input_file.is_open()) {
    std::cerr << "Error opening file for reading\n";
    return entries;
  }

  int size_of_entry = sizeof(models::Entry);
  models::Entry current_entry;
  std::vector<models::Entry> saved_entries{};

  std::unique_lock<std::mutex> lock{this->file_mutex};
  while (input_file.read(reinterpret_cast<char *>(&current_entry),
                         size_of_entry)) {
    if (comparison(current_entry)) {
      entries.push_back(current_entry);
    }
  }
  input_file.close();
  lock.release();

  return entries;
}

void FileManagement::file_search(std::stop_token stoken) {
  std::ifstream input_file(this->storage_file);

  while (!stoken.stop_requested()) {
    std::vector<
        std::pair<models::Task, std::promise<std::vector<models::Entry>>>>
        items;
    {
      std::unique_lock<std::mutex> lock(this->file_search_mutex);
      this->search_cond_var.wait(
          lock, stoken, [&] { return !this->file_search_queue.empty(); });

      if (this->file_search_queue.empty()) {
        continue;
      }

      items = std::move(this->file_search_queue);
      this->file_search_queue.clear();
    }

    for (auto &task : items) {
      std::vector<models::Entry> entries =
          this->search_in_file(task.first.comparison, input_file);

      task.second.set_value(entries);
    }
  }
}
