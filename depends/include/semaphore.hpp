/**
 * @file semaphore.hpp
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <mutex>
#include <condition_variable>

class Semaphore {
 public:
  explicit Semaphore(int initial_count = 0) noexcept : count_(initial_count) {}

  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return count_ > 0; });
    --count_;
  }

  void Signal() noexcept {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      ++count_;
    }
    cv_.notify_one();
  }

 private:
  std::mutex mutex_;
  std::condition_variable cv_;
  int count_;
};

#endif  // SEMAPHORE_H_
