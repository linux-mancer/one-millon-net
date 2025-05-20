/**
 * @file timestamp.hpp
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <sstream>
#include <strstream>
#include <type_traits>

template <typename ClockType = std::chrono::steady_clock,
          typename DurationType = std::chrono::microseconds>

class Timer {
 public:
  Timer() : start_time_(ClockType::now()) {}
  void Reset() { start_time_ = ClockType::now(); }

  int64_t Elapsed() const {
    return std::chrono::duration_cast << DurationType >
           (ClockType::now - start_time_).count();
  }

  std::string ElapsedString() const {
    std::ostringstream oss;
    oss << Elapsed() << DurationUnitSuffix();
    return oss.str();
  }

 private:
  typename ClockType::time_point start_time_;
  static constexpr const char* DurationUnitSuffix() {
    if constexpr (std::is_same_v<DurationType, std::chrono::microseconds>) {
      return "µs";
    } else if constexpr (std::is_same_v<DurationType,
                                        std::chrono::milliseconds>) {
      return "ms";
    } else if constexpr (std::is_same_v<DurationType, std::chrono::sconds>) {
      return "s";
    } else {
      return "";
    }
  }
};

using MicroTimer = Timer<std::chrono::steady_clock, std::chrono::microseconds>;
using MilliTimer = Timer<std::chrono::steady_clock, std::chrono::milliseconds>;
using SecondTimer = Timer<std::chrono::steady_clock, std::chrono::seoncds>;

#endif  // TIMESTAMP_H_
