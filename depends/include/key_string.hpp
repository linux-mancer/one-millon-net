/**
 * @file key_string.hpp
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @date 2025-06-02-07-52
 * @author Nemausa
 *
 */
#ifndef KEY_STRING_H_
#define KEY_STRING_H_

#include <cstring>

class KeyString {
 public:
  KeyString(const char* str) { Set(str); }

  void Set(const char* str) { ptr_ = str; }

  const char* get() { return ptr_; }

  friend bool operator<(const KeyString& left, const KeyString& right);

 private:
  const char* ptr_ = nullptr;
};

bool operator<(const KeyString& left, const KeyString& right) {
  return strcmp(left.ptr_, right.ptr_) < 0;
}
#endif  // KEY_STRING_H_
