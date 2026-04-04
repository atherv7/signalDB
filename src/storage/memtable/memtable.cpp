#include "memtable.h"

#include <cstdlib>
#include <mutex>
#include <vector>

#include "storage/models.h"

MemTable::MemTable(int memtable_capacity, int max_level, float probability)
    : memtable_capacity(memtable_capacity),
      max_level(max_level),
      probability(probability),
      memtable_size(0),
      current_level(0) {
  this->queue = new std::vector<models::Entry>();
  this->memtable_head = nullptr;
}

void MemTable::insert_queue(const std::vector<models::Entry>& entries) {
  std::lock_guard<std::mutex> lock(this->lock);
  this->queue->insert(this->queue->end(), entries.begin(), entries.end());
}

void MemTable::from_queue_to_memtable() {
  std::vector<models::Entry>* to_memtable = nullptr;
  {
    std::lock_guard<std::mutex> lock(this->lock);
    to_memtable = this->queue;
    this->queue = new std::vector<models::Entry>();
  }

  for (models::Entry& entry : *to_memtable) {
    this->insert_memtable(entry);
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

void MemTable::flush_memtable() {}

auto MemTable::random_level() const -> int {
  int level = 0;
  while (static_cast<float>(std::rand()) / RAND_MAX < this->probability &&
         level < this->max_level) {
    ++level;
  }
  return level;
}
