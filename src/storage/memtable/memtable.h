#pragma once
#include "models.h"
#include <string>
#include <vector>

class MemTable {
public:
  MemTable(int capacity, std::string storage_file);

  void insert(models::Entry entry);

  bool delete_entry(models::Entry &entry);

  bool contains(models::Entry &entry);

private:
  std::vector<models::Entry> buffer;
  int entry_to_write;
  int capacity;
};
