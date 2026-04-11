#include "file_manager.h"

#include <cassert>
#include <cstdint>

#include "storage/models.h"

FileManager::FileManager(const std::string& storage_file_name) {
  this->file = new FileStore{storage_file_name};
}

void FileManager::add_to_file(const std::vector<models::Timestamp>& delta_delta_time,
                              const std::vector<int>& delta_values) {
  assert(delta_delta_time.size() == delta_values.size());

  uint32_t num_entries = static_cast<uint32_t>(delta_delta_time.size());
  uint32_t timestamps_size = num_entries * sizeof(models::Timestamp);
  uint32_t values_size = num_entries * sizeof(int);

  // write header
  file->write_async(&num_entries, sizeof(num_entries));
  file->write_async(&timestamps_size, sizeof(timestamps_size));
  file->write_async(&values_size, sizeof(values_size));

  // write raw data blocks
  file->write_async(delta_delta_time.data(), timestamps_size);
  file->write_async(delta_values.data(), values_size);
}
