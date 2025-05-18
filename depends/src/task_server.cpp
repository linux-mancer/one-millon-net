#include "task_server.h"
#include <mutex>
#include "server.h"
#include "thread.hpp"

void TaskServer::AddTask(Task task) {
  std::lock_guard<std::mutex> lock(mutex_);
  tasks_buffer_.push_back(task);
}

void TaskServer::Start() {
  thread_.Start(nullptr, [this](Thread* pthread) { RunLoop(pthread); });
}

void TaskServer::set_server_id(int id) {
  server_id_ = id;
}
void TaskServer::Stop() { thread_.Stop(); }

void TaskServer::RunLoop(Thread* thread) {
  while (thread->IsRunning()) {
    if (!tasks_buffer_.empty()) {
      std::lock_guard<std::mutex> lock(mutex_);
      for (auto task : tasks_buffer_) {
        tasks_.push_back(task);
      }
      tasks_buffer_.clear();
    }
    if (tasks_.empty()) {
      Thread::Sleep(1);
      continue;
    }
    for (auto task : tasks_) {
      task();
    }
    tasks_.clear();
  }
  // 线程停止了，也要处理缓冲区的任务
  for (auto task : tasks_buffer_) {
    task();
  }
}
