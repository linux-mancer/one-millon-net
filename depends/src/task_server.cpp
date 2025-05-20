#include "task_server.h"
#include <iterator>
#include <mutex>

TaskServer::TaskServer(int server_id) : server_id_(server_id) {}

TaskServer::~TaskServer() { Stop(); }

void TaskServer::Start() {
  bool expected = false;
  if (running_.compare_exchange_strong(expected, true)) {
    worker_thread_ = std::thread(&TaskServer::Run, this);
  }
}

void TaskServer::Stop() {
  running_.store(false);
  cond_var_.notify_one();
  if (worker_thread_.joinable()) {
    worker_thread_.join();
  }
}

void TaskServer::AddTask(const std::function<void()>& task) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_.push(task);
  }
  cond_var_.notify_one();
}

void TaskServer::set_server_id(int id) {
  server_id_ = id;
}

void TaskServer::Run() {
  std::unique_lock<std::mutex> lock(mutex_);
  while (running_.load() || !tasks_.empty()) {
    cond_var_.wait(lock,
                   [this] { return !running_.load() || !tasks_.empty(); });

    while (!tasks_.empty()) {
      auto task = std::move(tasks_.front());
      tasks_.pop();
      lock.unlock();
      task();
      lock.lock();
    }
  }
}
