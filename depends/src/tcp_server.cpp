#include "tcp_server.h"
#include <arpa/inet.h>
#include <glog/logging.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <cstdio>
#include <type_traits>
#include "cell.h"
#include "config.hpp"
#include "network.hpp"

TcpServer::TcpServer()
    : sockfd_(INVALID_SOCKET),
      recv_count_(0),
      msg_count_(0),
      client_accept_(0),
      client_join_(0),
      send_buffer_size_(
          Config::GetInstance().GetInt("send_buffer_size", SEND_BUFF_SIZE)),
      recv_buffer_size_(
          Config::GetInstance().GetInt("recv_buffer_size", RECV_BUFF_SIZE)),
      max_clients_(Config::GetInstance().GetInt("max_client", 100000)) {}

TcpServer::~TcpServer() { Close(); }

SOCKET TcpServer::Init() {
  Netowrk::Init();
  if (INVALID_SOCKET != sockfd_) {
    LOG(INFO) << "Init close old socket " << sockfd_;
    Close();
  }

  sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (INVALID_SOCKET == sockfd_) {
    LOG(INFO) << "create socket failed..";
  } else {
    Netowrk::MakeReuseAddr(sockfd_);
    LOG(INFO) << "create socket " << sockfd_ << " success";
  }
  return sockfd_;
}

int TcpServer::Bind(const char *ip, uint16_t port) {
  LOG(INFO) << "ip<" << ip << "> port<" << port << ">";
  sockaddr_in sin = {};
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  if (ip) {
    sin.sin_addr.s_addr = inet_addr(ip);
  } else {
    sin.sin_addr.s_addr = INADDR_ANY;
  }

  Netowrk::MakeReuseAddr(sockfd_);

  int ret = ::bind(sockfd_, (sockaddr *)&sin, sizeof(sin));
  if (SOCKET_ERROR == ret) {
    LOG(ERROR) << "bind port " << port << " failed" << strerror(errno);
  } else {
    LOG(INFO) << "bind port " << port << " success";
  }

  return ret;
}

int TcpServer::Listen(int n) {
  int ret = listen(sockfd_, n);
  if (SOCKET_ERROR == ret) {
    LOG(ERROR) << "listen socket " << sockfd_ << " failed";
  } else {
    LOG(ERROR) << "listen socket " << sockfd_ << " success";
  }
  return ret;
}

SOCKET TcpServer::Accept() {
  sockaddr_in client_addr = {};
  int len = sizeof(client_addr);
  SOCKET sock = INVALID_SOCKET;
  sock = accept(sockfd_, (sockaddr *)&client_addr, (socklen_t *)&len);
  if (INVALID_SOCKET == sock) {
    LOG(ERROR) << "accept INVALID_SOCKET";
  } else {
    if (client_accept_ < max_clients_) {
      ++client_accept_;
      Netowrk::MakeReuseAddr(sock);
      AddClientToServer(new Client(sock, send_buffer_size_, recv_buffer_size_));
    } else {
      Netowrk::DestroySocket(sock);
      LOG(INFO) << "Accept to MaxClient";
    }
  }
  return sock;
}

void TcpServer::AddClientToServer(Client *client_ptr) {
  auto min_server = servers_[0];
  for (auto server : servers_) {
    if (min_server->GetClientCount() > server->GetClientCount()) {
      min_server = server;
    }
  }
  min_server->AddClient(client_ptr);
}

void TcpServer::Close() {
  LOG(INFO) << "TcpServer Close begin";
  thread_.Stop();
  if (sockfd_ != INVALID_SOCKET) {
    for (auto s : servers_) {
      delete s;
    }
    servers_.clear();
    Netowrk::DestroySocket(sockfd_);
    sockfd_ = INVALID_SOCKET;
  }
  LOG(INFO) << "TcpServer Close end";
}

void TcpServer::OnClientConnected(Client *client_ptr) { ++client_join_; }

void TcpServer::OnClientDisconnected(Client *client_ptr) {
  --client_accept_;
  --client_join_;
}
void TcpServer::OnDataReceived(Client *client) { ++recv_count_; }

void TcpServer::OnMessageReceived(Server *server, Client *client,
                                  DataHeader *header) {
  ++msg_count_;
}

void TcpServer::PrintMsg() {
  auto t = timer_.Elapsed() / 1000.0;
  if (t > 1.0) {
    LOG(INFO) << "thread " << servers_.size() << " time " << t << " socket "
              << sockfd_ << " accept " << client_accept_ << " join "
              << client_join_ << " recv " << recv_count_ << " msg "
              << msg_count_;
    recv_count_ = 0;
    msg_count_ = 0;
    timer_.Reset();
  }
}

SOCKET TcpServer::sockfd() { return sockfd_; }
