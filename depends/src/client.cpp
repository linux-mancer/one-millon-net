#include "client.h"
#include "cell.h"
#include "network.hpp"

Client::Client(SOCKET sock_fd, int send_size, int recv_size)
    : send_buffer_(send_size), recv_buffer_(recv_size) {
  static int n = 1;
  id_ = n++;
  socket_fd_ = sock_fd;
  ResetHeartbeat();
  ResetSendbeat();
}

Client::~Client() { Close(); }

void Client::Close() {
  if (INVALID_SOCKET != socket_fd_) {
    Network::DestroySocket(socket_fd_);
    socket_fd_ = INVALID_SOCKET;
  }
}

SOCKET Client::socket_fd() { return socket_fd_; }

int Client::RecvData() { return recv_buffer_.ReadFromSocket(socket_fd_); }

bool Client::HasMsg() const { return recv_buffer_.HasData(); }

DataHeader *Client::FrontMsg() { return (DataHeader *)recv_buffer_.data(); }

void Client::PopFrontMsg() {
  if (HasMsg()) {
    recv_buffer_.Consume(FrontMsg()->data_length);
  }
}

bool Client::NeedWrite() const { return send_buffer_.NeedWrite(); }

int Client::SendDataReal() {
  ResetSendbeat();
  return send_buffer_.WriteToSocket(socket_fd_);
}

int Client::SendData(const DataHeader *header) {
  return SendData((const char *)header, header->data_length);
}

int Client::SendData(const char *data, size_t length) {
  if (send_buffer_.Push(data, length)) {
    return length;
  }
  return SOCKET_ERROR;
}

void Client::ResetHeartbeat() { dt_heart_ = 0; }

void Client::ResetSendbeat() { dt_send_ = 0; }

bool Client::CheckHeart(time_t dt) {
  dt_heart_ += dt;
  // LOG(INFO) << "CheckHeart" << dt_heart_;
  if (dt_heart_ >= kClientHeartDeadTime) {
    LOG(INFO) << "CheckHeart dead:s=" << socket_fd_ << " time=" << dt_heart_;
    return true;
  }
  return false;
}

bool Client::CheckSend(time_t dt) {
  dt_send_ += dt;
  if (dt_send_ >= kClientSendBuffTime) {
    SendDataReal();
    ResetSendbeat();
    return true;
  }

  return false;
}

int Client::recv_msg_id() { return recv_msg_id_; }

int Client::recv_msg_id_next() { return ++recv_msg_id_; }

int Client::send_msg_id() { return send_msg_id_; }

int Client::send_msg_id_next() { return ++send_msg_id_; }
