#include <gtest/gtest.h>
#include <unistd.h>

#include <vector>

#include "file_management/file_manager.h"
#include "helpers.h"
#include "storage/models.h"
#include "storage/storage.h"

TEST(StorageTest, WriteAheadLog) {
  std::string storage_file = "file_storage_14.txt.ahead";
  helpers::File file{storage_file};
  FileManager file_manager("dummy_file");
  Storage* store = new Storage("file_storage_14.txt", file_manager);
  std::vector<models::Entry> entries = {
      models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2}};
  store->write_ahead_insert(entries);

  EXPECT_TRUE(file.wait_for_file());

  std::vector<models::Entry> saved_entries = helpers::get_entries_from_file(storage_file);

  EXPECT_TRUE(saved_entries.size() == 1);
  EXPECT_TRUE(saved_entries[0] == entries[0]);
}
