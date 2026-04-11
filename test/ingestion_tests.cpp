#include <gtest/gtest.h>
#include <pthread.h>

#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "file_management/file_manager.h"
#include "helpers.h"
#include "ingestion/server.h"
#include "storage/models.h"
#include "storage/storage.h"

TEST(APITests, PostRequest) {
  std::string storage_file = "api_storage.txt.ahead";
  helpers::File file{storage_file};
  FileManager file_manager("dummy_file.txt");
  std::shared_ptr<Storage> store = std::make_shared<Storage>("api_storage.txt", file_manager);
  Server server{store};

  std::thread server_thread([&]() { server.run(); });

  std::vector<models::Entry> entries = {
      {.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2}};

  EXPECT_TRUE(helpers::post_request(entries));
  EXPECT_TRUE(file.wait_for_file());

  std::vector<models::Entry> saved_entries = helpers::get_entries_from_file(storage_file);

  EXPECT_TRUE(saved_entries.size() == 1);
  EXPECT_TRUE(saved_entries[0] == entries[0]);

  // TODO: shouldn't need to do this
  file.~File();
  server.shutdown();
  if (server_thread.joinable()) {
    server_thread.join();
  }
}

TEST(APITests, PostWS) {
  std::string storage_file = "api_storage_2.txt.ahead";
  helpers::File file{storage_file};
  FileManager file_manager("dummy_file");
  std::shared_ptr<Storage> store = std::make_shared<Storage>("api_storage_2.txt", file_manager);
  Server server{store};

  std::thread server_thread([&]() { server.run(); });

  std::vector<models::Entry> entries = {
      {.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2},
      {.time = models::Timestamp{.hour = 1, .min = 2}, .value = 3},
      {.time = models::Timestamp{.hour = 2, .min = 3}, .value = 4},
      {.time = models::Timestamp{.hour = 3, .min = 4}, .value = 5},
  };

  EXPECT_TRUE(helpers::post_ws(entries));
  EXPECT_TRUE(file.wait_for_file());

  std::vector<models::Entry> saved_entries = helpers::get_entries_from_file(storage_file);

  EXPECT_TRUE(saved_entries.size() == 4);

  for (int i = 0; i < saved_entries.size(); i++) {
    EXPECT_EQ(saved_entries[i], entries[i]);
  }

  file.~File();
  server.shutdown();
  if (server_thread.joinable()) {
    server_thread.join();
  }
}
