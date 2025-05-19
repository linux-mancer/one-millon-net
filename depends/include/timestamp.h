/**
 * @file timestamp.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @date 2025-05-19-19-34
 * @author Nemausa
 *
 */
#ifndef timestamp_H_
#define timestamp_H_

#include <chrono>
class Time {
 public:
  static int64_t NowInMilliseconds();
};

class Timestamp {
 public:
  Timestamp();
  void Reset();
  double ElapsedSeconds() const;
  double ElapseMilliseconds() const;
  double ElapsedMircoseconds() const;

 private:
  std::chrono::high_resolution_clock::time_point start_time_;
};

#endif  // timestamp_H_
