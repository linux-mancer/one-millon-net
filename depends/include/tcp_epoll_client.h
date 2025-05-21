/**
 * @file tpc_epoll_client.hpp
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef TPC_EPOLL_CLIENT_H_
#define TPC_EPOLL_CLIENT_H_

#include <asm-generic/errno.h>
#include <sys/epoll.h>
#include "cell.h"
#include "client.h"
#include "tcp_client.h"
#include "epoll_wrapper.h"

class TcpEpollClient : public TcpClient {
 public:
  void OnInitSocket() override;
  void Close() override;
  bool Run(int timeout_us) override;

 private:
  EpollWrapper epoll_wrapper_;
};

#endif  // TPC_EPOLL_CLIENT_H_
