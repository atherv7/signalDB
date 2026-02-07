#include "file_management.h"
#include "storage/models.h"

auto FileManagement::has_in_file(models::Entry& entry) -> std::future<std::vector<models::Entry>> {
  std::promise<std::vector<models::Entry>> result;
  std::future<std::vector<models::Entry>> fut = result.get_future();
  models::Task t{[&entry](const models::Entry& e) { return e == entry; }};

  this->queue_task(t, result);
  this->search_cond_var.notify_one();

  return fut;
}

auto FileManagement::get_before(models::Timestamp& time)
    -> std::future<std::vector<models::Entry>> {
  std::promise<std::vector<models::Entry>> result;
  std::future<std::vector<models::Entry>> fut = result.get_future();
  models::Task t{[&time](const models::Entry& entry) { return entry.time < time; }};

  this->queue_task(t, result);
  this->search_cond_var.notify_one();

  return fut;
}

auto FileManagement::get_after(models::Timestamp& time) -> std::future<std::vector<models::Entry>> {
  std::promise<std::vector<models::Entry>> result;
  std::future<std::vector<models::Entry>> fut = result.get_future();

  models::Task t{[&time](const models::Entry& entry) { return entry.time > time; }};

  this->queue_task(t, result);
  this->search_cond_var.notify_one();
  return fut;
}

auto FileManagement::get_between(models::Timestamp& before_time, models::Timestamp& after_time)
    -> std::future<std::vector<models::Entry>> {
  std::promise<std::vector<models::Entry>> result;
  std::future<std::vector<models::Entry>> fut = result.get_future();

  models::Task t{[&before_time, &after_time](const models::Entry& entry) {
    return entry.time < after_time && entry.time > before_time;
  }};

  this->queue_task(t, result);
  this->search_cond_var.notify_one();
  return fut;
}

void FileManagement::file_search(std::stop_token& stoken) {
  while (!stoken.stop_requested()) {
    std::vector<std::pair<models::Task, std::promise<std::vector<models::Entry>>>> items;
    {
      std::unique_lock<std::mutex> lock(this->file_search_mutex);
      this->search_cond_var.wait(lock, stoken, [&] { return !this->file_search_queue.empty(); });

      if (this->file_search_queue.empty()) {
        continue;
      }

      items = std::move(this->file_search_queue);
      this->file_search_queue.clear();
    }

    for (auto& task : items) {
      std::vector<models::Entry> entries = this->file_store->search_entries(task.first.comparison);

      task.second.set_value(entries);
    }
  }
}

void FileManagement::queue_task(models::Task& task,
                                std::promise<std::vector<models::Entry>>& result) {
  std::unique_lock<std::mutex> lock{this->file_search_mutex};
  this->file_search_queue.emplace_back(std::move(task), std::move(result));
}
