#include "memtable.h"

#include <cstdlib>
#include <mutex>
#include <thread>

MemTable::MemTable(int memtable_capacity, int max_level, float probability)
    : memtable_capacity{memtable_capacity},
      max_level{max_level},
      probability{probability},
      memtable_size{0},
      current_level{0},
      memtable_running{false} {
  this->queue = new std::vector<models::Entry>();
  this->memtable_head = nullptr;
}

MemTable::~MemTable() {
  this->stop();
}

void MemTable::insert(const std::vector<models::Entry>& entries) {
  std::lock_guard<std::mutex> lock(this->queue_lock);
  this->queue->insert(this->queue->end(), entries.begin(), entries.end());
}

void MemTable::start() {
  this->memtable_running = true;
  this->queue_to_memtable_thread = std::thread(&MemTable::from_queue_to_memtable, this);
  this->memtable_to_file_thread = std::thread(&MemTable::flush_memtable, this);
}

void MemTable::stop() {
  {
    std::lock_guard<std::mutex> lock(this->queue_lock);
    this->memtable_running = false;
  }

  this->queue_to_memtable_cv.notify_one();

  if (this->queue_to_memtable_thread.joinable()) {
    this->queue_to_memtable_thread.join();
  }

  if (this->memtable_to_file_thread.joinable()) {
    this->memtable_to_file_thread.join();
  }
}

void MemTable::from_queue_to_memtable() {
  while (true) {
    {
      std::unique_lock<std::mutex> lock(this->queue_lock);
      this->queue_to_memtable_cv.wait(
          lock, [this] { return not this->memtable_running || not this->queue->empty(); });

      if (not this->memtable_running && this->queue->empty()) {
        return;
      }
    }
    std::vector<models::Entry>* to_memtable = nullptr;
    {
      std::lock_guard<std::mutex> lock(this->queue_lock);
      to_memtable = this->queue;
      this->queue = new std::vector<models::Entry>();
    }

    {
      std::lock_guard<std::mutex> lock(this->memtable_lock);
      for (models::Entry& entry : *to_memtable) {
        this->insert_memtable(entry);
      }
    }

    this->memtable_size += to_memtable->size();
  }
}

void MemTable::insert_memtable(models::Entry entry) {
  std::vector<MemTableNode*> update(this->max_level + 1, nullptr);
  MemTableNode* curr = this->memtable_head;

  for (int lvl = this->current_level; lvl >= 0; --lvl) {
    while (curr->next[lvl] && curr->next[lvl]->entry < entry) {
      curr = curr->next[lvl];
    }
    update[lvl] = curr;
  }

  curr = curr->next[0];

  if (curr && curr->entry == entry) {
    return;
  }

  int new_level = this->random_level();
  if (new_level > this->current_level) {
    for (int lvl = this->current_level + 1; lvl <= new_level; ++lvl) {
      update[lvl] = this->memtable_head;
    }
    this->current_level = new_level;
  }

  MemTableNode* new_node = new MemTableNode(entry, new_level);
  for (int lvl = 0; lvl <= new_level; ++lvl) {
    new_node->next[lvl] = update[lvl]->next[lvl];
    update[lvl]->next[lvl] = new_node;
  }
}

void MemTable::flush_memtable() {
  MemTableNode* to_be_flushed = nullptr;

  // TODO: mutex may not be necessary if this
  // is triggered after an insertion
  {
    std::lock_guard<std::mutex> lock(this->memtable_lock);
    to_be_flushed = this->memtable_head;
    this->memtable_head = nullptr;
  }

  std::vector<int> delta_values = {};
  std::vector<models::Timestamp> delta_time = {};

  while (to_be_flushed) {
    models::Entry entry = to_be_flushed->entry;
    models::Timestamp time = entry.time;
    int val = entry.value;
    if (delta_values.empty()) {
      delta_values.push_back(val);
    } else {
      delta_values.push_back(val - delta_values.back());
    }

    if (delta_time.empty()) {
      delta_time.push_back(time);
    } else {
      delta_time.push_back(time - delta_time.back());
    }

    to_be_flushed = to_be_flushed->next[0];
  }

  std::vector<models::Timestamp> delta_delta_time(delta_time.size(), models::Timestamp{});
  delta_delta_time[0] = delta_time[0];
  for (int ind = 1; ind < delta_delta_time.size(); ++ind) {
    delta_delta_time[ind] = delta_time[ind] - delta_time[ind - 1];
  }
}

auto MemTable::random_level() const -> int {
  int level = 0;
  while (static_cast<float>(std::rand()) / RAND_MAX < this->probability &&
         level < this->max_level) {
    ++level;
  }
  return level;
}
