#include "epoll_tcp_client.h"
#include <sched.h>
#include "cell.h"

void EpollTcpClient::OnInitSocket() {
  epoll_.Init(1);
  epoll_.Register(client()->sock_fd(), EPOLLIN, client());
}

void EpollTcpClient::Close() {
  epoll_.Close();
  TcpClient::Close();
}

bool EpollTcpClient::Run(int timeout_us) {
  if (!is_connected()) {
    return false;
  }

  Client* client_ptr = client();
  uint32_t events = EPOLLIN;
  if (client_ptr->NeedWrite()) {
    events |= EPOLLOUT;
  }
  epoll_.Modify(client_ptr->sock_fd(), events, client_ptr);

  int n = epoll_.Wait(timeout_us);
  if (n < 0) {
    LOG(ERROR) << "EpollTcpClient Run wait failed";
    Close();
    return false;
  }

  if (n == 0) {
    return true;
  }

  const auto& buffer = epoll_.events();
  for (int i = 0; i < n; ++i) {
    Client* cli = static_cast<Client*>(buffer[i].data.ptr);
    if (!cli) continue;

    if ((buffer[i].events & EPOLLIN) && SOCKET_ERROR == RecvData()) {
      // LOG(ERROR) << "RecvData error";
      Close();
      continue;
    }

    if (buffer[i].events & EPOLLOUT && SOCKET_ERROR == cli->SendDataReal()) {
      // LOG(ERROR) << "SendDataReal exit";
      Close();
    }
  }

  return true;
}
