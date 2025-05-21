#include "tcp_epoll_client.h"

void TcpEpollClient::OnInitSocket() {
  epoll_wrapper_.Init(1);
  epoll_wrapper_.Register(client()->sock_fd(), EPOLLIN, client());
}

void TcpEpollClient::Close() {
  epoll_wrapper_.Close();
  TcpClient::Close();
}

bool TcpEpollClient::Run(int timeout_us) {
  if (!is_connected()) {
    return false;
  }

  Client* client_ptr = client();
  uint32_t events = EPOLLIN;
  if (client_ptr->NeedWrite()) {
    events |= EPOLLOUT;
  }

  epoll_wrapper_.Modify(client_ptr->sock_fd(), events, client_ptr);

  int n = epoll_wrapper_.Wait(timeout_us);
  if (n < 0) {
    LOG(ERROR) << "TcpEpollClient Run ";
    return false;
  }

  if (n == 0) {
    return true;
  }

  const auto& buffer = epoll_wrapper_.events();
  for (int i = 0; i < n; ++i) {
    Client* client_ptr = static_cast<Client*>(buffer[i].data.ptr);
    if (!client_ptr) continue;
    if (buffer[i].events & EPOLLIN) {
      if (SOCKET_ERROR == RecvData()) {
        LOG(ERROR) << "RecvData error";
        Close();
        continue;
      }
    }

    if (buffer[i].events & EPOLLOUT) {
      if (SOCKET_ERROR == client_ptr->SendDataReal()) {
        LOG(ERROR) << "SendDataReal exit";
        Close();
      }
    }
  }

  return true;
}
