#include "split_string.h"
#include <cstring>

void SplitString::Reset(char* buffer) { ptr_ = buffer; }

char* SplitString::Next(char delim) {
  if (ptr_ == nullptr || *ptr_ == '\0') {
    return nullptr;
  }

  char* found = static_cast<char*>(::memchr(ptr_, delim, std::strlen(ptr_)));
  if (found != nullptr) {
    *found = '\0';
    char* token = ptr_;
    ptr_ = found + 1;
    return token;
  }

  // not found delim, return all as token
  char* token = ptr_;
  ptr_ = nullptr;
  return token;
}

char* SplitString::Next(const char* delim) {
  if (ptr_ == nullptr || *ptr_ == '\0' || delim == nullptr || *delim == '\0') {
    return nullptr;
  }

  char* found = std::strstr(ptr_, delim);
  if (found != nullptr) {
    *found = '\0';
    char* token = ptr_;
    ptr_ = found + std::strlen(delim);
    return token;
  }

  char* token = ptr_;
  ptr_ = nullptr;
  return token;
}
