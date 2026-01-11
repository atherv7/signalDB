#ifndef STORAGE
#define STORAGE
#include <functional>
#include <iostream>
#include <ostream>
#include <vector>

struct Timestamp {
  int hour;
  double min;

  bool operator==(const Timestamp &other_time) const {
    return hour == other_time.hour && min == other_time.min;
  }

  bool operator<(const Timestamp &other_time) const {
    double this_total = (hour * 60) + min;
    double other_total = (other_time.hour * 60) + other_time.min;

    return this_total < other_total;
  }

  bool operator>(const Timestamp &other_time) const {
    return other_time < *this;
  }

  friend std::ostream &operator<<(std::ostream &os, const Timestamp &time) {
    os << "Timestamp(hour: " << time.hour << ", minute: " << time.min << ")";
    return os;
  }
};

struct Entry {
  Timestamp time;
  int value;

  bool operator==(const Entry &other_entry) const {
    return time == other_entry.time && value == other_entry.value;
  }

  friend std::ostream &operator<<(std::ostream &os, const Entry &ent) {
    os << "Entry(timestamp: " << ent.time << ", value: " << ent.value << ")";
    return os;
  }

  Entry clone() {
    return Entry{
        .time = Timestamp{.hour = time.hour, .min = time.min},
        .value = value,
    };
  }
};

class Storage {
private:
  std::vector<Entry> storage;
  int capacity;

  /*
   * flush in-memory storage to file
   */
  void flush();

  std::vector<Entry>
  search_in_file(std::function<bool(const Entry &)> comparison);

public:
  Storage(int capacity);

  /*
   * insert entry into storage
   */
  void insert(Entry entry);

  /*
   * get entries before timestamp
   */
  std::vector<Entry> get_before(Timestamp &time);

  /*
   * get entries after timestamp
   */
  std::vector<Entry> get_after(Timestamp &time);

  /*
   * get entries in between timestamps
   */
  std::vector<Entry> get_between(Timestamp &before_time, Timestamp &after_time);

  /*
   * delete entry from storage
   */
  bool delete_entry(Entry &entry);

  /*
   * print current in memory storage
   */
  void print_storage();
};

#endif // STORAGE
