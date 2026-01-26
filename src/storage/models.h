#pragma once
#include <cstdint>
#include <fstream>
#include <functional>

namespace models {
struct Timestamp {
  int hour = -1;
  double min = -1;

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
  int value = -1;

  bool operator==(const Entry &other_entry) const {
    return time == other_entry.time && value == other_entry.value;
  }

  bool operator!=(const Entry &other_entry) const {
    return !(*this == other_entry);
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

struct Task {
  std::function<bool(const models::Entry &)> comparison;
};
} // namespace models
