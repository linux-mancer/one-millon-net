#include "epoll_server.h"
#include <sys/epoll.h>
#include <cstdint>
#include "cell.h"
#include "client.h"
#include <glog/logging.h>

EpollServer::~EpollServer() { Close(); }

void EpollServer::SetClientCapacity(int max_clients) {
  epoll_.Init(max_clients);
}

bool EpollServer::ProcessNetworkEvents() {
  for (const auto& [fd, client] : clients()) {
    uint32_t mask = EPOLLIN;
    if (client->NeedWrite()) {
      mask |= EPOLLOUT;
    }
    epoll_.Modify(fd, mask, client);
  }

  int n = epoll_.Wait(1);
  if (n < 0) {
    LOG(ERROR) << "EpollServer ProcessNetworkEvents wait";
    return false;
  } else if (n == 0) {
    // 超时，无事件
    return true;
  }

  auto events = epoll_.events();
  for (int i = 0; i < n; ++i) {
    auto& ev = events[i];
    Client* pclient = static_cast<Client*>(ev.data.ptr);
    int fd = pclient->socket_fd();

    if (ev.events & (EPOLLERR | EPOLLHUP)) {
      LOG(INFO) << "Client fd=" << fd << " disconnected (err/hub)";
      // RemoveClient(pclient);
      continue;
    }

    if ((ev.events & EPOLLIN) && ReceiveData(pclient) == SOCKET_ERROR) {
      // LOG(ERROR) << "Receive error on fd=" << fd;
      RemoveClient(pclient);
      continue;
    }

    if ((ev.events & EPOLLOUT) && pclient->SendDataReal() == SOCKET_ERROR) {
      // LOG(ERROR) << "Send error on fd=" << fd;
      RemoveClient(pclient);
      continue;
    }
  }
  return true;
}

void EpollServer::RemoveClient(Client* client) {
  int fd = client->socket_fd();
  epoll_.Unregister(fd);
  client->Close();
  clients().erase(fd);
  OnClientDisconnected(client);
}

void EpollServer::OnClientConnected(Client* client) {
  epoll_.Register(client->socket_fd(), EPOLLIN, client);
}
