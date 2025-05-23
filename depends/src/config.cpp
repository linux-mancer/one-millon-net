#include "config.hpp"
#include <cstdlib>
#include <memory>
#include <system_error>

void Config::Init(int argc, char* argv[]) noexcept {
  if (argc > 0) {
    exe_path_ = argv[0];
  }
  for (int i = 1; i < argc; ++i) {
    Parse(argv[i]);
  }
}

void Config::Parse(const std::string& cmd) noexcept {
  auto pos = cmd.find('=');
  if (pos != std::string::npos) {
    std::string key = cmd.substr(0, pos);
    std::string val = cmd.substr(pos + 1);
    kv_[key] = val;
    LOG(INFO) << "Config parsed " << key << " " << val;
  } else {
    kv_[cmd] = "";
    LOG(INFO) << "Config parsed flag " << cmd;
  }
}

bool Config::HasKey(const std::string& key) const noexcept {
  return kv_.count(key) > 0;
}

std::string Config::GetString(const std::string& key,
                              const std::string& default_value) const {
  auto it = kv_.find(key);
  if (it == kv_.end()) {
    LOG(INFO) << "Config GetString " << key
              << " not found, use default=" << default_value;
    return default_value;
  }
  LOG(INFO) << "Config GetString " << key << "=" << it->second;
  return it->second;
}

int Config::GetInt(const std::string& key, int default_value) const noexcept {
  auto it = kv_.find(key);
  if (it != kv_.end()) {
    try {
      int v = std::stoi(it->second);
      LOG(INFO) << "Config GetInt " << key << "=" << v;
      return v;
    } catch (const std::exception&) {
      LOG(ERROR) << "Config GetInt invalid int for " << key << " = "
                 << it->second;
    }
  }
  LOG(INFO) << "Config GetInt " << key
            << " not found, use default=" << default_value;
  return default_value;
}

bool Config::GetBool(const std::string& key,
                     bool default_value) const noexcept {
  auto str = GetString(key, default_value ? "true" : "false");
  if (str == "1" || str == "true" || str == "True") return true;
  if (str == "0" || str == "false" || str == "False") return false;
  LOG(ERROR) << "Config GetBool invalid boolean for " << key << " " << str;
  return default_value;
}

double Config::GetDouble(const std::string& key,
                         double default_value) const noexcept {
  auto it = kv_.find(std::string(key));
  if (it == kv_.end()) {
    LOG(INFO) << "Config GetDouble " << key << " not found, use default "
              << default_value;
    return default_value;
  }
  double result = default_value;
  try {
    result = std::stod(it->second);
  } catch (const std::exception& e) {
    LOG(ERROR) << "Config GetDouble: invalid double for " << key << " = "
               << it->second.c_str();
    result = default_value;
  }
  LOG(INFO) << "Config GetDouble: " << key << " = " << result;
  return result;
}
