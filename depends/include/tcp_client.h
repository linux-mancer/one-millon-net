/**
 * @file tcp_client.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

#include <bits/stdint-uintn.h>
#include <memory>
#include <string>
#include <cstdint>
#include "client.h"
#include "message_header.h"

class TcpClient {
 public:
  virtual bool Run(int timeout_us = 1000) = 0;

  TcpClient();
  ~TcpClient();

  int CreateSocket(int send_buffer_size = 10240, int recv_buffer_size = 10240);
  int Connect(const std::string& ip, uint16_t port);
  int Send(const DataHeader* header);
  int Send(const char* data, size_t length);
  bool is_connected() const { return static_cast<bool>(client_) && connected_; }
  int RecvData();
  Client* client() const { return client_.get(); }

 protected:
  virtual void OnInitSocket() {}
  virtual void OnConnect() {}
  virtual void OnMessage(DataHeader* header) = 0;
  virtual void Close();

 private:
  void DispatchMessages();

 private:
  std::unique_ptr<Client> client_;
  bool connected_ = false;
};

#endif  // TCP_CLIENT_H_
