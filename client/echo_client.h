/**
 * @file echo_client.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef ECHO_CLIENT_H_
#define ECHO_CLIENT_H_

#include <bits/stdint-uintn.h>
#include <bits/types/time_t.h>
#include <atomic>
#include <list>
#include <vector>

#include "cell.h"
#include "epoll_tcp_client.h"
#include "config.hpp"
#include "message_header.h"
#include "thread.hpp"

class EchoClient : public EpollTcpClient {
 public:
  EchoClient();
  virtual void OnMessage(DataHeader* header) override;
  int SendTest(LoginRequest* login);
  bool CheckSend(int dt);

  int reset_time();
  void set_reset_time(int t);
 private:
  int reset_time_ = 0;
  int recv_msg_id_ = 1;
  int send_msg_id_ = 1;
  int send_count_ = 0;
  bool check_msg_id_ = false;
  bool send_ = false;
};

#endif  // ECHO_CLIENT_H_
