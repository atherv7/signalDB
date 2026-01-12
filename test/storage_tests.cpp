#include "storage.hpp"
#include <gtest/gtest.h>

TEST(StorageTest, InsertStorage) {
  Storage *store = new Storage(2);
  store->insert(Entry{
      .time = Timestamp{.hour = 0, .min = 1},
      .value = 2,
  });
  store->insert(Entry{
      .time = Timestamp{.hour = 3, .min = 4},
      .value = 5,
  });
}
