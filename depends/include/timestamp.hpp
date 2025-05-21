#ifndef TIMESTAMP_HPP
#define TIMESTAMP_HPP

#include <chrono>
#include <cstdint>
#include <string>
#include <sstream>
#include <type_traits>

template <typename D>
struct DurationSuffix {
  static constexpr const char* value = "";
};

template <>
struct DurationSuffix<std::chrono::microseconds> {
  static constexpr const char* value = "µs";
};

template <>
struct DurationSuffix<std::chrono::milliseconds> {
  static constexpr const char* value = "ms";
};

template <>
struct DurationSuffix<std::chrono::seconds> {
  static constexpr const char* value = "s";
};

template <typename ClockType = std::chrono::steady_clock,
          typename DurationType = std::chrono::microseconds>
class Timer {
 public:
  Timer() noexcept : start_time_(ClockType::now()) {}
  ~Timer() = default;

  void Reset() noexcept { start_time_ = ClockType::now(); }

  DurationType ElapsedDuration() const noexcept {
    return std::chrono::duration_cast<DurationType>(ClockType::now() -
                                                    start_time_);
  }

  int64_t Elapsed() const noexcept { return ElapsedDuration().count(); }

  std::string ElapsedString() const {
    std::ostringstream oss;
    oss << Elapsed() << DurationSuffix<DurationType>::value;
    return oss.str();
  }

 private:
  typename ClockType::time_point start_time_;
};

using MicroTimer = Timer<std::chrono::steady_clock, std::chrono::microseconds>;
using MilliTimer = Timer<std::chrono::steady_clock, std::chrono::milliseconds>;
using SecondTimer = Timer<std::chrono::steady_clock, std::chrono::seconds>;

#endif  // TIMESTAMP_HPP
