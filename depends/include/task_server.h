/**
 * @file task_server.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @date 2025-05-19 22:37
 * @author Nemausa
 */

#ifndef TASK_SERVER_H_
#define TASK_SERVER_H_


#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

/**
 * @brief A simple threaded task server.
 *
 * Spawns a single worker thread that executes added tasks in FIFO order.
 */
class TaskServer {
 public:
  /**
   * @brief Constructs a TaskServer.
   * @param server_id Identifier of this server instance.
   */
  explicit TaskServer(int server_id);

  // Non-copyable, non-movable.
  TaskServer(const TaskServer&) = delete;
  TaskServer& operator=(const TaskServer&) = delete;

  /**
   * @brief Destroys the TaskServer, stops the thread if still running.
   */
  ~TaskServer();

  /**
   * @brief Starts processing tasks.
   *
   * Must be called before AddTask().
   */
  void Start();

  /**
   * @brief Stops processing tasks and joins the thread.
   *
   * Blocks until the worker thread exits.
   */
  void Stop();

  /**
   * @brief Adds a task to be executed asynchronously.
   *
   * Thread-safe; notifies the worker thread to wake up.
   * @param task Callable with no arguments and no return value.
   */
  void AddTask(const std::function<void()>& task);

 private:
  /** Main loop run by the worker thread. */
  void Run();

  const int server_id_;               ///< Server identifier.
  std::atomic<bool> running_{false};  ///< Controls the Run loop.
  std::thread worker_thread_;         ///< The worker thread.

  std::mutex mutex_;                         ///< Protects tasks_.
  std::condition_variable cond_var_;         ///< Signals new tasks or shutdown.
  std::queue<std::function<void()>> tasks_;  ///< Pending tasks.
};

#endif  // TASK_SERVER_H_
