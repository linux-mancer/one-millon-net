/**
 * @file echo_server.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef ECHO_SERVER_H_
#define ECHO_SERVER_H_

#include "epoll_tcp_server.h"
#include "server.h"

class EchoServer : public EpollTcpServer {
 public:
  EchoServer();
  virtual void OnClientConnected(Client*) override;
  virtual void OnClientDisconnected(Client*) override;
  virtual void OnMessageReceived(Server* server, Client*, DataHeader*) override;

 private:
  bool send_back_;
  bool send_full_;
  bool check_msg_id_;
};

#endif  // ECHO_SERVER_H_
