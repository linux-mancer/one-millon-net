#include "tcp_client.h"
#include <arpa/inet.h>
#include <glog/logging.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <memory>
#include "cell.h"
#include "client.h"
#include "message_header.h"
#include "network.hpp"

TcpClient::TcpClient() = default;
TcpClient::~TcpClient() { Close(); }

int TcpClient::CreateSocket(int send_buffer_size, int recv_buffer_size) {
  Network::Init();
  if (client_) {
    LOG(INFO) << "Reinitializing socket, closing old socket="
              << client_->socket_fd();
    Close();
  }

  int sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0) {
    PLOG(ERROR) << "socket() failed";
    return SOCKET_ERROR;
  }

  Network::MakeReuseAddr(sock);
  client_ = new Client(sock, send_buffer_size, recv_buffer_size);
  OnInitSocket();

  return sock;
}

int TcpClient::Connect(const std::string &ip, uint16_t port) {
  if (!client_ && CreateSocket() == SOCKET_ERROR) {
    return SOCKET_ERROR;
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (::inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
    LOG(ERROR) << "Invalid IP address: " << ip;
    return SOCKET_ERROR;
  }

  int ret = ::connect(client_->socket_fd(), reinterpret_cast<sockaddr *>(&addr),
                      sizeof(addr));
  if (ret < 0) {
    PLOG(ERROR) << "connect(" << ip << ":" << port << ") failed";
  } else {
    connected_ = true;
    OnConnect();
  }
  return ret;
}

void TcpClient::Close() {
  if (client_) {
    delete client_;
    client_ = nullptr;
  }
  connected_ = false;
}

void TcpClient::DispatchMessages() {
  while (client_->HasMsg()) {
    OnMessage(client_->FrontMsg());
    client_->PopFrontMsg();
  }
}

int TcpClient::Send(const DataHeader *header) {
  if (!is_connected()) return SOCKET_ERROR;
  return client_->SendData(header);
}

int TcpClient::Send(const char *data, size_t length) {
  if (!is_connected()) return SOCKET_ERROR;
  return client_->SendData(data, length);
}

int TcpClient::RecvData() {
  if (is_connected()) {
    int len = client_->RecvData();
    if (len > 0) {
      DispatchMessages();
    }
    return len;
  }
  return 0;
}

bool TcpClient::is_connected() const { return client_ && connected_; }
