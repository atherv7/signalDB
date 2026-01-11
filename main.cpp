#include "storage.hpp"
#include <iostream>
#include <vector>

int main() {
  Storage *store{new Storage(1)};

  store->insert(Entry{.time = Timestamp{.hour = 1, .min = 20}, .value = 4});
  store->insert(Entry{.time = Timestamp{.hour = 0, .min = 2}, .value = 1});
  store->insert(Entry{.time = Timestamp{.hour = 2, .min = 45}, .value = 8});

  Timestamp time_check = Timestamp{.hour = 0, .min = 10};
  std::vector<const Entry *> after_35 = store->get_after(time_check);

  std::cout << "======== After 35 =============" << std::endl;
  for (auto ent : after_35) {
    std::cout << *ent << std::endl;
  }

  Entry entry_to_delete =
      Entry{.time = Timestamp{.hour = 0, .min = 2}, .value = 1};
  bool deleted = store->delete_entry(entry_to_delete);

  std::cout << "======== deleted ============" << std::endl;
  std::cout << "deleted: " << deleted << std::endl;

  std::cout << "========= current storage ======" << std::endl;
  store->print_storage();
}
