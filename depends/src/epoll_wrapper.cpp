#include "epoll_wrapper.h"
#include <bits/stdint-uintn.h>
#include <cstring>
#include "network.hpp"

EpollWrapper::EpollWrapper() {}

EpollWrapper::~EpollWrapper() { Close(); }

EpollWrapper::EpollWrapper(EpollWrapper&& other) noexcept {
  epoll_fd_ = other.epoll_fd_;
  max_events_ = other.max_events_;
  events_ = std::move(other.events_);
  other.epoll_fd_ = -1;
  other.max_events_ = 0;
}

EpollWrapper& EpollWrapper::operator=(EpollWrapper&& other) noexcept {
  if (this != &other) {
    Close();
    epoll_fd_ = other.epoll_fd_;
    max_events_ = other.max_events_;
    events_ = other.events_;
    other.epoll_fd_ = -1;
    other.max_events_ = 0;
  }
  return *this;
}

int EpollWrapper::Init(int max_events) {
  if (epoll_fd_ > 0) {
    Close();
  }
  epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
  if (epoll_fd_ < 0) {
    LOG(ERROR) << "epoll_create1 failed: " << std::strerror(errno);
    return epoll_fd_;
  }
  events_.clear();
  events_.resize(max_events);
  max_events_ = max_events;
  return epoll_fd_;
}

void EpollWrapper::Close() {
  if (epoll_fd_ > 0) {
    Netowrk::DestroySocket(epoll_fd_);
    epoll_fd_ = -1;
  }
  events_.clear();
}

bool EpollWrapper::Control(int op, int fd, uint32_t events, void* ptr) {
  epoll_event ev{};
  ev.events = events;
  if (ptr) {
    ev.data.ptr = ptr;
  } else {
    ev.data.fd = fd;
  }
  if (::epoll_ctl(epoll_fd_, op, fd, &ev) < 0) {
    LOG(ERROR) << "epoll_ctl_op=" << op << " fd=" << fd
               << " err=" << std::strerror(errno);
    return false;
  }
  return true;
}

bool EpollWrapper::Register(int fd, uint32_t events, void* ptr) {
  return Control(EPOLL_CTL_ADD, fd, events, ptr);
}

bool EpollWrapper::Modify(int fd, uint32_t events, void* ptr) {
  return Control(EPOLL_CTL_MOD, fd, events, ptr);
}

bool EpollWrapper::Unregister(int fd) {
  return Control(EPOLL_CTL_DEL, fd, 0, nullptr);
}

int EpollWrapper::Wait(int timeout_ms) {
  int n = ::epoll_wait(epoll_fd_, events_.data(), max_events_, timeout_ms);
  if (n < 0) {
    if (errno == EINTR) {
      LOG(WARNING) << "epoll_wait interrupted by signal";
      return 0;
    }
    LOG(ERROR) << "epoll_wai failed: " << std::strerror(errno);
    return -1;
  }
  return n;
}
