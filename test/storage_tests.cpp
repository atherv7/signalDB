#include "storage.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <unistd.h>
#include <vector>

TEST(StorageTest, InsertStorage) {
  std::string storage_file = "file_storage_1.txt";
  Storage *store = new Storage(2, storage_file);
  store->insert(Entry{
      .time = Timestamp{.hour = 0, .min = 1},
      .value = 2,
  });
  store->insert(Entry{
      .time = Timestamp{.hour = 3, .min = 4},
      .value = 5,
  });

  std::string correct_output =
      "Entry(timestamp: Timestamp(hour: 0, minute: 1), "
      "value: 2)\nEntry(timestamp: Timestamp(hour: 3, "
      "minute: 4), value: 5)\n";

  EXPECT_EQ(correct_output, store->to_string());

  std::filesystem::remove(storage_file);
}

TEST(StorageTest, HasEntry) {
  std::string storage_file = "file_storage_2.txt";
  Storage *store = new Storage(2, storage_file);
  store->insert(Entry{
      .time = Timestamp{.hour = 0, .min = 1},
      .value = 3,
  });

  Entry entry = Entry{.time = Timestamp{.hour = 0, .min = 1}, .value = 3};

  EXPECT_TRUE(store->has_entry(entry));

  std::filesystem::remove(storage_file);
}

TEST(StorageTest, GetBefore) {
  std::string storage_file = "file_storage_3.txt";
  Storage *store = new Storage(2, storage_file);
  store->insert(Entry{
      .time = Timestamp{.hour = 0, .min = 1},
      .value = 2,
  });
  store->insert(Entry{
      .time = Timestamp{.hour = 3, .min = 4},
      .value = 5,
  });

  Timestamp before_time = Timestamp{.hour = 0, .min = 20};

  std::vector<Entry> before_entries = store->get_before(before_time);

  Entry correct_entry{
      Entry{.time = Timestamp{.hour = 0, .min = 1}, .value = 2}};

  EXPECT_EQ(before_entries.size(), 1);
  EXPECT_EQ(before_entries[0], correct_entry);

  std::filesystem::remove(storage_file);
}

TEST(StorageTest, GetAfter) {
  std::string storage_file = "file_storage_4.txt";
  Storage *store = new Storage(2, storage_file);
  store->insert(Entry{
      .time = Timestamp{.hour = 0, .min = 1},
      .value = 2,
  });
  store->insert(Entry{
      .time = Timestamp{.hour = 3, .min = 4},
      .value = 5,
  });

  Timestamp after_time = Timestamp{.hour = 0, .min = 20};

  std::vector<Entry> after_entries = store->get_after(after_time);

  Entry correct_entry{
      Entry{.time = Timestamp{.hour = 3, .min = 4}, .value = 5}};

  EXPECT_EQ(after_entries.size(), 1);
  EXPECT_EQ(after_entries[0], correct_entry);

  std::filesystem::remove(storage_file);
}

TEST(StorageTest, GetBetween) {
  std::string storage_file = "file_storage_5.txt";
  Storage *store = new Storage(2, storage_file);
  store->insert(Entry{.time = Timestamp{.hour = 0, .min = 1}, .value = 2});
  store->insert(Entry{.time = Timestamp{.hour = 3, .min = 4}, .value = 5});

  Timestamp before_time = Timestamp{.hour = 0, .min = 0};
  Timestamp after_time = Timestamp{.hour = 5, .min = 0};

  std::vector<Entry> between_entries =
      store->get_between(before_time, after_time);

  EXPECT_EQ(between_entries.size(), 2);

  std::filesystem::remove(storage_file);
}

TEST(StorageTest, DeleteEntry) {
  std::string storage_file = "file_storage_6.txt";
  Storage *store = new Storage(2, storage_file);
  store->insert(Entry{
      .time = Timestamp{.hour = 2, .min = 1},
      .value = 2,
  });

  Entry entry_to_delete = Entry{
      .time = Timestamp{.hour = 2, .min = 1},
      .value = 2,
  };

  store->delete_entry(entry_to_delete);

  EXPECT_FALSE(store->has_entry(entry_to_delete));

  std::filesystem::remove(storage_file);
}

TEST(StorageTest, InsertStorageFile) {
  std::string storage_file = "file_storage_7.txt";
  Storage *store = new Storage(1, storage_file);
  store->insert(Entry{.time = Timestamp{.hour = 0, .min = 1}, .value = 2});
  store->insert(Entry{.time = Timestamp{.hour = 3, .min = 4}, .value = 5});

  Entry entry_in_file =
      Entry{.time = Timestamp{.hour = 0, .min = 1}, .value = 2};

  std::ifstream input_file(storage_file, std::ios::binary | std::ios::in);
  if (!input_file.is_open()) {
    std::cerr << "Error opening file for reading" << std::endl;
    return;
  }

  int size_of_entry = sizeof(Entry);
  Entry current_entry;
  std::vector<Entry> saved_entries{};

  while (input_file.read(reinterpret_cast<char *>(&current_entry),
                         size_of_entry)) {
    saved_entries.push_back(current_entry);
  }
  input_file.close();

  EXPECT_TRUE(saved_entries.size() == 1);
  EXPECT_TRUE(saved_entries[0] == entry_in_file);

  std::filesystem::remove(storage_file);
}

TEST(StorageTest, HasEntryInFile) {
  std::string storage_file = "file_storage_9.txt";
  Storage *store = new Storage(1, storage_file);
  store->insert(Entry{.time = Timestamp{.hour = 0, .min = 1}, .value = 2});
  store->insert(Entry{.time = Timestamp{.hour = 3, .min = 4}, .value = 5});

  Entry entry_in_file =
      Entry{.time = Timestamp{.hour = 0, .min = 1}, .value = 2};

  EXPECT_TRUE(store->has_entry(entry_in_file));

  std::filesystem::remove(storage_file);
}

TEST(StorageTest, GetBeforeInFile) {
  std::string storage_file = "file_storage_10.txt";
  Storage *store = new Storage(1, storage_file);
  store->insert(Entry{.time = Timestamp{.hour = 0, .min = 1}, .value = 2});
  store->insert(Entry{.time = Timestamp{.hour = 3, .min = 4}, .value = 5});

  Timestamp time = Timestamp{.hour = 1, .min = 0};
  std::vector<Entry> before_entries = store->get_before(time);

  Entry file_entry = Entry{.time = Timestamp{.hour = 0, .min = 1}, .value = 2};

  EXPECT_TRUE(before_entries.size() == 1);
  EXPECT_TRUE(before_entries[0] == file_entry);
}

TEST(StorageTest, GetAfterInFile) {
  std::string storage_file = "file_storage_11.txt";
  Storage *store = new Storage(1, storage_file);
  store->insert(Entry{.time = Timestamp{.hour = 3, .min = 4}, .value = 5});
  store->insert(Entry{.time = Timestamp{.hour = 0, .min = 1}, .value = 2});

  Timestamp time = Timestamp{.hour = 1, .min = 0};
  std::vector<Entry> before_entries = store->get_after(time);

  Entry file_entry = Entry{.time = Timestamp{.hour = 3, .min = 4}, .value = 5};

  EXPECT_TRUE(before_entries.size() == 1);
  EXPECT_TRUE(before_entries[0] == file_entry);

  std::filesystem::remove(storage_file);
}

TEST(StorageTest, GetBetweenInFile) {
  std::string storage_file = "file_storage_12.txt";
  Storage *store = new Storage(1, storage_file);

  Entry first_entry = Entry{.time = Timestamp{.hour = 3, .min = 4}, .value = 5};
  Entry second_entry =
      Entry{.time = Timestamp{.hour = 0, .min = 1}, .value = 2};

  store->insert(first_entry);
  store->insert(second_entry);

  Timestamp before_time = Timestamp{.hour = 0, .min = 0};
  Timestamp after_time = Timestamp{.hour = 5, .min = 0};

  std::vector<Entry> between_entries =
      store->get_between(before_time, after_time);

  EXPECT_TRUE(between_entries.size() == 2);
  EXPECT_TRUE(between_entries[0] == first_entry);
  EXPECT_TRUE(between_entries[1] == second_entry);

  std::filesystem::remove(storage_file);
}

TEST(StorageTest, DeleteEntryInFile) {
  std::string storage_file = "file_storage_13.txt";
  Storage *store = new Storage(1, storage_file);

  store->insert(Entry{.time = Timestamp{.hour = 0, .min = 1}, .value = 2});
  store->insert(Entry{.time = Timestamp{.hour = 3, .min = 4}, .value = 5});

  Entry entry_to_delete =
      Entry{.time = Timestamp{.hour = 0, .min = 1}, .value = 2};

  store->delete_entry(entry_to_delete);

  EXPECT_FALSE(store->has_entry(entry_to_delete));

  std::filesystem::remove(storage_file);
}
