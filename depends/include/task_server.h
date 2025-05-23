/**
 * @file task_server.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef TASK_SERVER_H_
#define TASK_SERVER_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <list>
#include "thread.hpp"

class TaskServer {
  using Task = std::function<void()>;

 public:
  void AddTask(Task task);
  void Start();
  void Stop();
  void server_id(int id);

 private:
  void RunLoop(Thread *thread);

 private:
  int server_id_;
  std::list<Task> tasks_;
  std::list<Task> tasks_buffer_;
  std::mutex mutex_;
  Thread thread_;
};

#endif  // TASK_SERVER_H_
