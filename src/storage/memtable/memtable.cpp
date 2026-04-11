#include "memtable.h"

#include <cstdlib>
#include <memory>
#include <mutex>
#include <stop_token>
#include <thread>
#include <utility>
#include <vector>

#include "file_management/file_manager.h"
#include "storage/models.h"

MemTable::MemTable(int memtable_capacity,
                   FileManager& file_manager,
                   int max_level,
                   float probability)
    : file_manager{file_manager},
      memtable_capacity{memtable_capacity},
      max_level{max_level},
      probability{probability},
      memtable_size{0},
      current_level{0} {
  this->memtable_head = nullptr;
}

void MemTable::insert(const std::vector<models::Entry>& entries) {
  std::lock_guard<std::mutex> lock(this->queue_lock);
  this->queue.insert(this->queue.end(), entries.begin(), entries.end());
}

void MemTable::start() {
  this->queue_to_memtable_thread =
      std::jthread([this](std::stop_token st) { this->from_queue_to_memtable(std::move(st)); });
}

void MemTable::stop() {
  {
    std::lock_guard<std::mutex> lock(this->queue_lock);
  }

  this->queue_to_memtable_cv.notify_one();

  if (this->queue_to_memtable_thread.joinable()) {
    this->queue_to_memtable_thread.join();
  }

  if (this->memtable_to_file_thread.joinable()) {
    this->memtable_to_file_thread.join();
  }
}

void MemTable::from_queue_to_memtable(std::stop_token stoken) {
  while (!stoken.stop_requested()) {
    std::vector<models::Entry> local_batch;
    {
      std::unique_lock lock(this->queue_lock);
      this->queue_to_memtable_cv.wait(
          lock, [&] { return stoken.stop_requested() || not this->queue.empty(); });

      if (this->queue.empty()) {
        continue;
      }

      local_batch = std::move(this->queue);
      this->queue.clear();
    }

    std::lock_guard lock(this->memtable_lock);
    for (auto& entry : local_batch) {
      this->insert_memtable(entry);
      this->memtable_size += sizeof(entry);
    }

    if (this->memtable_size >= this->memtable_capacity) {
      // TODO: transfer to thread
      this->flush_memtable();
    }
  }
}

void MemTable::insert_memtable(models::Entry entry) {
  std::vector<MemTableNode*> update(this->max_level + 1, this->memtable_head.get());
  MemTableNode* curr = this->memtable_head.get();

  for (int lvl = this->max_level; lvl >= 0; --lvl) {
    while (curr->levels[lvl] && curr->levels[lvl]->entry < entry) {
      curr = curr->levels[lvl];
    }
    update[lvl] = curr;
  }

  int new_level = this->random_level();
  auto new_node = std::make_unique<MemTableNode>(entry, new_level);
  MemTableNode* new_node_ptr = new_node.get();

  new_node->next_owned = std::move(update[0]->next_owned);
  update[0]->next_owned = std::move(new_node);

  for (int lvl = 0; lvl <= new_level; ++lvl) {
    new_node_ptr->levels[lvl] = update[lvl]->levels[lvl];
    update[lvl]->levels[lvl] = new_node_ptr;
  }
}

void MemTable::flush_memtable() {
  std::unique_ptr<MemTableNode> old_data;

  {
    std::lock_guard<std::mutex> lock(this->memtable_lock);
    if (not this->memtable_head->next_owned) {
      return;
    }

    old_data = std::move(this->memtable_head->next_owned);

    this->memtable_size = 0;
  }

  std::vector<int> delta_values;
  std::vector<models::Timestamp> delta_time;

  MemTableNode* curr = old_data.get();

  while (curr) {
    const models::Entry& entry = curr->entry;

    if (delta_values.empty()) {
      delta_values.push_back(entry.value);
    } else {
      delta_values.push_back(entry.value - delta_values.back());
    }

    if (delta_time.empty()) {
      delta_time.push_back(entry.time);
    } else {
      delta_time.push_back(entry.time - delta_time.back());
    }
  }

  if (delta_time.empty()) {
    return;
  }

  std::vector<models::Timestamp> delta_delta_time(delta_time.size());
  delta_delta_time[0] = delta_time[0];
  for (size_t i = 1; i < delta_time.size(); ++i) {
    delta_delta_time[i] = delta_time[i] - delta_time[i - 1];
  }

  this->file_manager.add_to_file(delta_delta_time, delta_values);
}

auto MemTable::random_level() const -> int {
  int level = 0;
  while (static_cast<float>(std::rand()) / RAND_MAX < this->probability &&
         level < this->max_level) {
    ++level;
  }
  return level;
}
