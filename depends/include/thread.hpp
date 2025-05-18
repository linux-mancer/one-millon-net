/**
 * @file thread.hpp
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <chrono>
#include "semaphore.hpp"

class Thread {
 public:
  using Callback = std::function<void(Thread*)>;
  // explicit Thread(Callback on_create = nullptr, Callback on_run = nullptr,
  //                 Callback on_destroy = nullptr) noexcept
  //     : on_create_(std::move(on_create)),
  //       on_run_(std::move(on_run)),
  //       on_destroy_(std::move(on_destroy)) {}
  ~Thread() { Stop(); }

  static void Sleep(time_t dt) {
    std::chrono::milliseconds t(dt);
    std::this_thread::sleep_for(t);
  }

  void Start(Callback on_create = nullptr, Callback on_run = nullptr,
             Callback on_destroy = nullptr) {
    bool expected = false;
    if (running_.compare_exchange_strong(expected, true)) {
      on_create_ = on_create;
      on_run_ = on_run;
      on_destroy_ = on_destroy;
      worker_ = std::thread(&Thread::ThreadFunc, this);
      worker_.detach();
    }
  }

  void Stop() noexcept {
    if (running_) {
      {
        std::lock_guard<std::mutex> lk(mutex_);
        exit_requested = true;
      }
      running_ = false;
      semaphore_.Wait();
    }
  }

  void Exit() {
    if (running_) {
      std::lock_guard<std::mutex> lk(mutex_);
      running_ = false;
    }
  }

  bool IsRunning() const noexcept { return running_.load(); }

  static void SleepFor(std::chrono::milliseconds ms) noexcept {
    std::this_thread::sleep_for(ms);
  }

 private:
  void ThreadFunc() {
    if (on_create_) {
      on_create_(this);
    }
    if (on_run_) {
      on_run_(this);
    }
    if (on_destroy_) {
      on_destroy_(this);
    }
    semaphore_.Signal();
    running_ = false;
  }

 private:
  std::thread worker_;
  std::atomic<bool> running_{false};
  std::atomic<bool> exit_requested{false};
  std::mutex mutex_;
  // std::condition_variable cv_;
  Semaphore semaphore_;

  Callback on_create_;
  Callback on_run_;
  Callback on_destroy_;
};

#endif  // THREAD_H_
