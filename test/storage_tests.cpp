#include <gtest/gtest.h>
#include <unistd.h>

#include <fstream>
#include <vector>

#include "helpers.h"
#include "storage/models.h"
#include "storage/storage.h"

TEST(StorageTest, WriteAheadLog) {
  std::string storage_file = "file_storage_14.txt.ahead";
  helpers::File file{storage_file};
  Storage* store = new Storage("file_storage_14.txt");
  std::vector<models::Entry> entries = {
      models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2}};
  store->write_ahead_insert(entries);

  EXPECT_TRUE(file.wait_for_file());

  std::ifstream input_file(storage_file, std::ios::binary | std::ios::in);
  if (!input_file.is_open()) {
    std::cerr << "Error opening file for reading\n";
    FAIL();
  }

  int size_of_entry = sizeof(models::Entry);
  models::Entry current_entry;
  std::vector<models::Entry> saved_entries{};

  while (input_file.read(reinterpret_cast<char*>(&current_entry), size_of_entry)) {
    saved_entries.push_back(current_entry);
  }
  input_file.close();

  EXPECT_TRUE(saved_entries.size() == 1);
  EXPECT_TRUE(saved_entries[0] == entries[0]);
}
