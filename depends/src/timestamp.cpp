#include "timestamp.h"
#include <bits/stdint-intn.h>
#include <chrono>

int64_t Time::NowInMilliseconds() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::high_resolution_clock::now().time_since_epoch())
      .count();
}

Timestamp::Timestamp() { Reset(); }

void Timestamp::Reset() {
  start_time_ == std::chrono::high_resolution_clock::now();
}

double Timestamp::ElapsedSeconds() const {
  return ElapsedMircoseconds() * 1e-6;
}

double Timestamp::ElapseMilliseconds() const {
  return ElapsedMircoseconds() * 1e-3;
}

double Timestamp::ElapsedMircoseconds() const {
  auto now = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(now -
                                                               start_time_)
      .count();
}
