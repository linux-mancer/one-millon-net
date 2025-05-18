#include "epoll_wrapper.h"
#include <bits/stdint-uintn.h>
#include <sys/epoll.h>
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
  max_events_ = max_events;
  epoll_fd_ = epoll_create(max_events_);
  if (epoll_fd_ < 0) {
    LOG(ERROR) << "epoll_create failed: " << std::strerror(errno);
    return epoll_fd_;
  }
  events_ = new epoll_event[max_events_];
  return epoll_fd_;
}

void EpollWrapper::Close() {
  if (epoll_fd_ > 0) {
    Network::DestroySocket(epoll_fd_);
    epoll_fd_ = -1;
  }

  if (events_) {
    delete[] events_;
    events_ = nullptr;
  }
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
  int n;
  // 如果被信号中断（EINTR），自动重试
  do {
    n = ::epoll_wait(epoll_fd_, events_, max_events_, timeout_ms);
  } while (n < 0 && errno == EINTR);

  if (n < 0) {
    LOG(ERROR) << "epoll_wait failed: " << std::strerror(errno);
    return -1;
  }
  return n;
}

epoll_event* EpollWrapper::events() const { return events_; }

int EpollWrapper::max_events() const { return max_events_; }
