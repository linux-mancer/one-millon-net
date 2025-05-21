/**
 * @file epoll_tcp_server.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef EPOLL_TCP_SERVER_H_
#define EPOLL_TCP_SERVER_H_

#include "tcp_server.h"
#include "epoll_wrapper.h"
#include "epoll_server.h"
#include "thread.hpp"

class EpollTcpServer : public TcpServer {
 public:
  void Start(int server_count);

 protected:
  void Run(Thread* thread) override;
};

#endif  // EPOLL_TCP_SERVER_H_
