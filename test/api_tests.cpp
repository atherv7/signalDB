#include <gtest/gtest.h>

#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "api/server.h"
#include "helpers.h"
#include "storage/models.h"
#include "storage/storage.h"

TEST(APITests, PostRequest) {
  std::string storage_file = "api_storage.txt.ahead";
  helpers::File file{storage_file};
  std::shared_ptr<Storage> store = std::make_shared<Storage>(1, "api_storage.txt", 1);
  Server server{store};

  std::thread server_thread([&]() { server.run(); });

  std::vector<models::Entry> entries = {
      {.time = models::Timestamp{.hour = 0, .min = 1}, .value = 2}};

  EXPECT_TRUE(helpers::post_request(entries));
  EXPECT_TRUE(file.wait_for_file());

  std::ifstream input_file(storage_file, std::ios::binary | std::ios::in);
  if (not input_file.is_open()) {
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

  server.shutdown();
  if (server_thread.joinable()) {
    server_thread.join();
  }
}
