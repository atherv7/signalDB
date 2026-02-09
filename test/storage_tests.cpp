#include <gtest/gtest.h>
#include <unistd.h>

#include <fstream>
#include <vector>

#include "helpers.h"
#include "storage/models.h"
#include "storage/storage.h"

TEST(StorageTest, InsertStorage) {
  std::string storage_file = "file_storage_1.txt";
  helpers::File file{storage_file};
  Storage* store = new Storage(2, storage_file, 1);
  store->insert(models::Entry{
      .time = models::Timestamp{.hour = 0, .min = 1},
      .value = 2,
  });
  store->insert(models::Entry{
      .time = models::Timestamp{.hour = 3, .min = 4},
      .value = 5,
  });

  std::string correct_output =
      "Entry(time: Timestamp(hour: 0, minute: 1), "
      "value: 2)\nEntry(time: Timestamp(hour: 3, "
      "minute: 4), value: 5)\n";

  EXPECT_EQ(correct_output, store->to_string());
}

TEST(StorageTest, HasEntry) {
  std::string storage_file = "file_storage_2.txt";
  helpers::File file{storage_file};
  Storage* store = new Storage(2, storage_file, 1);
  store->insert(models::Entry{
      .time = models::Timestamp{.hour = 0, .min = 1},
      .value = 3,
  });

  models::Entry entry = models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value = 3};

  EXPECT_TRUE(store->has_entry(entry));
}

TEST(StorageTest, GetBefore) {
  std::string storage_file = "file_storage_3.txt";
  helpers::File file{storage_file};
  Storage* store = new Storage(2, storage_file, 1);
  store->insert(models::Entry{
      .time = models::Timestamp{.hour = 0, .min = 1},
      .value = 2,
  });
  store->insert(models::Entry{
      .time = models::Timestamp{.hour = 3, .min = 4},
      .value = 5,
  });

  models::Timestamp before_time = models::Timestamp{.hour = 0, .min = 20};

  std::vector<models::Entry> before_entries = store->get_before(before_time);

  models::Entry correct_entry{
      models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2}};

  EXPECT_EQ(before_entries.size(), 1);
  EXPECT_EQ(before_entries[0], correct_entry);
}

TEST(StorageTest, GetAfter) {
  std::string storage_file = "file_storage_4.txt";
  helpers::File file{storage_file};
  Storage* store = new Storage(2, storage_file, 1);
  store->insert(models::Entry{
      .time = models::Timestamp{.hour = 0, .min = 1},
      .value = 2,
  });
  store->insert(models::Entry{
      .time = models::Timestamp{.hour = 3, .min = 4},
      .value = 5,
  });

  models::Timestamp after_time = models::Timestamp{.hour = 0, .min = 20};

  std::vector<models::Entry> after_entries = store->get_after(after_time);

  models::Entry correct_entry{
      models::Entry{.time = models::Timestamp{.hour = 3, .min = 4}, .value = 5}};

  EXPECT_EQ(after_entries.size(), 1);
  EXPECT_EQ(after_entries[0], correct_entry);
}

TEST(StorageTest, GetBetween) {
  std::string storage_file = "file_storage_5.txt";
  helpers::File file{storage_file};
  Storage* store = new Storage(2, storage_file, 1);
  store->insert(models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2});
  store->insert(models::Entry{.time = models::Timestamp{.hour = 3, .min = 4}, .value = 5});

  models::Timestamp before_time = models::Timestamp{.hour = 0, .min = 0};
  models::Timestamp after_time = models::Timestamp{.hour = 5, .min = 0};

  std::vector<models::Entry> between_entries = store->get_between(before_time, after_time);

  EXPECT_EQ(between_entries.size(), 2);
}

// TEST(StorageTest, DeleteEntry) {
//   std::string storage_file = "file_storage_6.txt";
//   Storage* store = new Storage(2, storage_file, 1);
//   store->insert(models::Entry{
//       .time = models::Timestamp{.hour = 2, .min = 1},
//       .value = 2,
//   });
//
//   models::Entry entry_to_delete = models::Entry{
//       .time = models::Timestamp{.hour = 2, .min = 1},
//       .value = 2,
//   };
//
//   store->delete_entry(entry_to_delete);
//
//   EXPECT_FALSE(store->has_entry(entry_to_delete));
//
//   std::filesystem::remove(storage_file);
// }

TEST(StorageTest, InsertStorageFile) {
  std::string storage_file = "file_storage_7.txt";
  helpers::File file{storage_file};
  Storage* store = new Storage(1, storage_file, 1);
  store->insert(models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2});
  store->insert(models::Entry{.time = models::Timestamp{.hour = 3, .min = 4}, .value = 5});

  models::Entry entry_in_file =
      models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2};

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
  EXPECT_TRUE(saved_entries[0] == entry_in_file);
}

TEST(StorageTest, HasEntryInFile) {
  std::string storage_file = "file_storage_9.txt";
  helpers::File file{storage_file};
  Storage* store = new Storage(1, storage_file, 1);
  store->insert(models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2});
  store->insert(models::Entry{.time = models::Timestamp{.hour = 3, .min = 4}, .value = 5});

  EXPECT_TRUE(file.wait_for_file());

  models::Entry entry_in_file =
      models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2};

  EXPECT_TRUE(store->has_entry(entry_in_file));
}

TEST(StorageTest, GetBeforeInFile) {
  std::string storage_file = "file_storage_10.txt";
  helpers::File file{storage_file};
  Storage* store = new Storage(1, storage_file, 1);
  store->insert(models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2});
  store->insert(models::Entry{.time = models::Timestamp{.hour = 3, .min = 4}, .value = 5});

  EXPECT_TRUE(file.wait_for_file());

  models::Timestamp time = models::Timestamp{.hour = 1, .min = 0};
  std::vector<models::Entry> before_entries = store->get_before(time);

  models::Entry file_entry =
      models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2};

  EXPECT_EQ(before_entries.size(), 1);
  EXPECT_TRUE(before_entries[0] == file_entry);
}

TEST(StorageTest, GetAfterInFile) {
  std::string storage_file = "file_storage_11.txt";
  helpers::File file{storage_file};
  Storage* store = new Storage(1, storage_file, 1);
  store->insert(models::Entry{.time = models::Timestamp{.hour = 3, .min = 4}, .value = 5});
  store->insert(models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2});

  EXPECT_TRUE(file.wait_for_file());
  models::Timestamp time = models::Timestamp{.hour = 1, .min = 0};
  std::vector<models::Entry> before_entries = store->get_after(time);

  models::Entry file_entry =
      models::Entry{.time = models::Timestamp{.hour = 3, .min = 4}, .value = 5};

  EXPECT_TRUE(before_entries.size() == 1);
  EXPECT_TRUE(before_entries[0] == file_entry);
}

TEST(StorageTest, GetBetweenInFile) {
  std::string storage_file = "file_storage_12.txt";
  helpers::File file{storage_file};
  Storage* store = new Storage(1, storage_file, 1);

  models::Entry first_entry =
      models::Entry{.time = models::Timestamp{.hour = 3, .min = 4}, .value = 5};
  models::Entry second_entry =
      models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2};

  store->insert(first_entry);
  store->insert(second_entry);

  EXPECT_TRUE(file.wait_for_file());
  models::Timestamp before_time = models::Timestamp{.hour = 0, .min = 0};
  models::Timestamp after_time = models::Timestamp{.hour = 5, .min = 0};

  std::vector<models::Entry> between_entries = store->get_between(before_time, after_time);

  EXPECT_EQ(between_entries.size(), 2);
  EXPECT_TRUE(between_entries[0] == first_entry);
  EXPECT_TRUE(between_entries[1] == second_entry);
}

// TEST(StorageTest, DeleteEntryInFile) {
//   std::string storage_file = "file_storage_13.txt";
//   Storage *store = new Storage(1, storage_file, 1);
//
//   store->insert(models::Entry{.time = models::Timestamp{.hour = 0, .min = 1},
//                               .value = 2});
//   store->insert(models::Entry{.time = models::Timestamp{.hour = 3, .min = 4},
//                               .value = 5});
//
//   std::this_thread::sleep_for(std::chrono::milliseconds(300));
//
//   models::Entry entry_to_delete =
//       models::Entry{.time = models::Timestamp{.hour = 0, .min = 1}, .value =
//       2};
//
//   store->delete_entry(entry_to_delete);
//
//   EXPECT_FALSE(store->has_entry(entry_to_delete));
//
//   std::filesystem::remove(storage_file);
// }
//
TEST(StorageTest, WriteAheadLog) {
  std::string storage_file = "file_storage_14.txt.ahead";
  helpers::File file{storage_file};
  Storage* store = new Storage(1, "file_storage_14.txt", 1);
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
