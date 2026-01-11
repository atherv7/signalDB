#include "storage.hpp"
#include <iostream>
#include <vector>

void first_test() {
  Storage *store{new Storage(1)};

  store->insert(Entry{.time = Timestamp{.hour = 1, .min = 20}, .value = 4});
  store->insert(Entry{.time = Timestamp{.hour = 0, .min = 2}, .value = 1});
  store->insert(Entry{.time = Timestamp{.hour = 2, .min = 45}, .value = 8});

  Timestamp time_check = Timestamp{.hour = 0, .min = 10};
  std::vector<Entry> after_35 = store->get_after(time_check);

  std::cout << "======== After 35 =============" << std::endl;
  for (auto ent : after_35) {
    std::cout << ent << std::endl;
  }

  Entry entry_to_delete =
      Entry{.time = Timestamp{.hour = 0, .min = 2}, .value = 1};
  bool deleted = store->delete_entry(entry_to_delete);

  std::cout << "======== deleted ============" << std::endl;
  std::cout << "deleted: " << deleted << std::endl;

  std::cout << "========= current storage ======" << std::endl;
  store->print_storage();
}

void second_test() {
  Storage *store{new Storage(2)};

  store->insert(Entry{.time = Timestamp{.hour = 0, .min = 13}, .value = 0});
  store->insert(Entry{.time = Timestamp{.hour = 2, .min = 10}, .value = 1});
  store->insert(Entry{.time = Timestamp{.hour = 0, .min = 21}, .value = 2});
  store->insert(Entry{.time = Timestamp{.hour = 2, .min = 20}, .value = 3});

  Timestamp time_check = Timestamp{.hour = 1, .min = 0};
  std::vector<Entry> before = store->get_before(time_check);
  std::cout << "======== before =========" << std::endl;
  for (const auto entry : before) {
    std::cout << entry << std::endl;
  }

  std::vector<Entry> after = store->get_after(time_check);
  std::cout << "======= after ============" << std::endl;
  for (const auto entry : after) {
    std::cout << entry << std::endl;
  }

  // clean file_storage.txt
  std::remove("file_storage.txt");
}

int main() { second_test(); }
