#include "file_management.h"
#include "storage/models.h"

FileManagement::FileManagement(std::string& storage_file, int file_flush_cap)
    : flush_queue_cap{file_flush_cap} {
  this->file_store = new FileStore(storage_file);
  this->file_flush_worker_ =
      std::jthread([this](std::stop_token stoken) { this->flush_to_file(stoken); });
  this->file_search_worker_ =
      std::jthread([this](std::stop_token stoken) { this->file_search(stoken); });
}

auto FileManagement::has_file() const -> bool {
  return this->contains_file;
}

void FileManagement::insert_flush_queue(models::Entry entry) {
  std::lock_guard<std::mutex> lock(this->flush_queue_mutex);
  this->file_flush_queue.push_back(entry);
  this->flush_cond_var.notify_one();
}

void FileManagement::flush_to_file(std::stop_token& stoken) {
  while (!stoken.stop_requested()) {
    std::vector<models::Entry> batch;
    {
      std::unique_lock<std::mutex> lock{this->flush_queue_mutex};
      this->flush_cond_var.wait(
          lock, stoken, [this] { return this->file_flush_queue.size() >= this->flush_queue_cap; });

      if (stoken.stop_requested() && this->file_flush_queue.empty()) {
        continue;
      }

      batch = std::move(this->file_flush_queue);
      this->file_flush_queue.clear();
    }
    this->file_store->write_entries(batch);
    this->contains_file = true;
  }
}
