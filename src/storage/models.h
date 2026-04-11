#pragma once

#include <cmath>
#include <functional>
#include <iostream>
#include <nlohmann/json.hpp>

namespace models {
struct Timestamp {
  int hour = -1;
  double min = -1;

  bool operator==(const Timestamp& other_time) const {
    return hour == other_time.hour && min == other_time.min;
  }

  bool operator<(const Timestamp& other_time) const {
    double this_total = (hour * 60) + min;
    double other_total = (other_time.hour * 60) + other_time.min;

    return this_total < other_total;
  }

  bool operator>(const Timestamp& other_time) const { return other_time < *this; }

  Timestamp operator-(const Timestamp& other_time) const {
    double this_minutes = (this->hour * 60) + this->min;
    double other_minutes = (other_time.hour * 60) + other_time.min;

    double difference = this_minutes - other_minutes;

    int hours = int(difference / 60);
    double mins = std::fmod(difference, 60.0);

    return {hours, mins};
  }

  friend std::ostream& operator<<(std::ostream& os, const Timestamp& time) {
    os << "Timestamp(hour: " << time.hour << ", minute: " << time.min << ")";
    return os;
  }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Timestamp, hour, min);

struct Entry {
  Timestamp time;
  int value = -1;

  bool operator==(const Entry& other_entry) const {
    return time == other_entry.time && value == other_entry.value;
  }

  bool operator!=(const Entry& other_entry) const { return !(*this == other_entry); }

  bool operator<(const Entry& other_entry) const { return this->time < other_entry.time; }

  friend std::ostream& operator<<(std::ostream& os, const Entry& ent) {
    os << "Entry(time: " << ent.time << ", value: " << ent.value << ")";
    return os;
  }

  Entry clone() {
    return Entry{
        .time = Timestamp{.hour = time.hour, .min = time.min},
        .value = value,
    };
  }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Entry, time, value);

struct Task {
  std::function<bool(const models::Entry&)> comparison;
};
}  // namespace models
