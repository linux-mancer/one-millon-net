#include "fd_set_wrapper.h"
#include <glog/logging.h>
#include <algorithm>
#include <cstring>
#include <climits>

FdSetWrapper::FdSetWrapper(int max_fds)
    : max_fds_(max_fds), buffer_((max_fds_ + CHAR_BIT - 1) / CHAR_BIT, 0) {}

void FdSetWrapper::Clear() { std::fill(buffer_.begin(), buffer_.end(), 0); }

bool FdSetWrapper::Add(int fd) {
  if (fd < 0 || fd >= max_fds_) {
    LOG(ERROR) << "FdSetWrapper::Add invalid fd = " << fd
               << " max_fds=" << max_fds_;
    return false;
  }
  buffer_[fd / CHAR_BIT] |= static_cast<char>(1u << (fd % CHAR_BIT));
  return true;
}

void FdSetWrapper::Remove(int fd) {
  if (fd < 0 || fd >= max_fds_) {
    LOG(WARNING) << "FdSetWrapper::Remove invalid fd=" << fd;
    return;
  }
  buffer_[fd / CHAR_BIT] &= static_cast<char>(~(1u << (fd % CHAR_BIT)));
}

bool FdSetWrapper::Contains(int fd) const {
  if (fd < 0 || fd >= max_fds_) return false;
  return buffer_[fd / CHAR_BIT] & static_cast<char>(1u << (fd % CHAR_BIT));
}

fd_set* FdSetWrapper::Raw() {
  return reinterpret_cast<fd_set*>(buffer_.data());
}
