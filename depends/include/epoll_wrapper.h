/**
 * @file epoll_wrapper.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef EPOLL_WRAPPER_H_
#define EPOLL_WRAPPER_H_

#include "cell.h"

class EpollWrapper {
 public:
  EpollWrapper();
  ~EpollWrapper();

  EpollWrapper(const EpollWrapper &) = delete;
  EpollWrapper &operator=(const EpollWrapper &) = delete;
  EpollWrapper(EpollWrapper &&) noexcept;
  EpollWrapper &operator=(EpollWrapper &&) noexcept;

  int Init(int max_events);
  void Close();

  bool Register(int fd, uint32_t events, void *prt = nullptr);
  bool Modify(int fd, uint32_t events, void *ptr = nullptr);
  bool Unregister(int fd);

  int Wait(int timeout_ms);
  const std::vector<epoll_event> &events() const { return events_; }
  int max_events() const { return max_events_; }

 private:
  bool Control(int op, int fd, uint32_t events, void *ptr);
  int epoll_fd_{-1};
  int max_events_{0};
  std::vector<epoll_event> events_;
};

#endif  // EPOLL_WRAPPER_H_
