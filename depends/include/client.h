/**
 * @file client.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @date 2025-05-17-08-27
 * @author Nemausa
 *
 */
#ifndef client_H_
#define client_H_

#include <chrono>
#include <cstddef>
#include "buffer.h"
#include "message_header.h"

constexpr auto kClientHeartDeadTime = 120000;
constexpr auto kClientSendBuffTime = 200;

class Client {
 public:
  Client(SOCKET sock_fd, int send_size, int recv_size);
  ~Client();
  void Destroy();

  int RecvData();
  bool HasMsg() const;
  DataHeader* FrontMsg();
  void PopFrontMsg();

  bool NeedWrite() const;
  int SendDataReal();
  int SendData(const DataHeader* header);
  int SendData(const char* data, size_t len);

  void ResetHeartbeat();
  void ResetSendbeat();
  bool CheckHeart(time_t dt);
  bool CheckSend(time_t dt);

  SOCKET sock_fd();
  int recv_msg_id();
  int recv_msg_id_next();
  int send_msg_id();
  int send_msg_id_next();

 private:
  int id_ = -1;
  int server_id_ = -1;
  int recv_msg_id_ = 1;
  int send_msg_id_ = 1;

  SOCKET sock_fd_;
  Buffer send_buffer_;
  Buffer recv_buffer_;
  time_t dt_heart_;
  time_t dt_send_;
};

#endif  // client_H_
