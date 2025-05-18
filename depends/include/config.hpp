/**
 * @file config.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <charconv>
#include <string>
#include <string_view>
#include <unordered_map>
#include <glog/logging.h>

class Config {
 public:
  static Config& GetInstance() noexcept {
    static Config instance;
    return instance;
  }

  void Init(int argc, char* argv[]) noexcept;
  bool HasKey(const std::string& key) const noexcept;
  std::string GetString(const std::string& key,
                        const std::string& default_value = "") const;
  int GetInt(const std::string& key, int default_value = 0) const noexcept;
  bool GetBool(const std::string& key,
               bool default_value = false) const noexcept;
  double GetDouble(const std::string& key,
                   double default_value = 0.0) const noexcept;
  const std::string& exe_path() const noexcept { return exe_path_; }

 private:
  Config() = default;
  ~Config() = default;
  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;
  void Parse(const std::string& cmd) noexcept;

  std::string exe_path_;
  std::unordered_map<std::string, std::string> kv_;
};

#endif  // CONFIG_H_
