#include "file_management.h"
#include "storage/models.h"
#include <fstream>
#include <iostream>
#include <mutex>
#include <stop_token>
#include <thread>
#include <vector>

FileManagement::FileManagement(std::string &storage_file, int file_flush_cap)
    : storage_file{storage_file}, flush_queue_cap{file_flush_cap} {
  this->file_flush_worker_ = std::jthread(
      [this](std::stop_token stoken) { this->flush_to_file(stoken); });
  this->file_search_worker_ = std::jthread(
      [this](std::stop_token stoken) { this->file_search(stoken); });
}

auto FileManagement::has_file() -> const bool { return this->contains_file; }

void FileManagement::insert_flush_queue(models::Entry entry) {
  std::lock_guard<std::mutex> lock(this->flush_queue_mutex);
  this->file_flush_queue.push_back(entry);
}

void FileManagement::flush_to_file(std::stop_token stoken) {
  std::ofstream output_file(this->storage_file,
                            std::ios::binary | std::ios::out | std::ios::trunc);

  while (!stoken.stop_requested()) {
    std::vector<models::Entry> batch;
    {
      std::unique_lock<std::mutex> lock{this->flush_queue_mutex};
      this->flush_cond_var.wait(lock, stoken, [this] {
        return this->file_flush_queue.size() >= this->flush_queue_cap;
      });

      if (stoken.stop_requested() && this->file_flush_queue.empty()) {
        continue;
      }

      batch = std::move(this->file_flush_queue);
      this->file_flush_queue.clear();
    }

    this->write_to_file(batch, output_file);
    this->contains_file = true;
  }
}

// TODO: need to implement lock for the file
void FileManagement::write_to_file(std::vector<models::Entry> &entries,
                                   std::ofstream &output_file) {
  // TODO: check proper modes with how the ofstream is initialized
  output_file.open(this->storage_file, std::ios::app);
  if (!output_file.is_open()) {
    std::cerr << "Error: unable to open file for writing\n";
    return;
  }

  int size_of_entry = sizeof(models::Entry);

  std::unique_lock<std::mutex> lock{this->file_mutex};
  for (const auto &entry : entries) {
    output_file.write(reinterpret_cast<const char *>(&entry), size_of_entry);
  }
  output_file.close();
  lock.release();

  entries.clear();
}
