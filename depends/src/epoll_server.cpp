#include "epoll_server.h"
#include <sys/epoll.h>
#include "client.h"

EpollServer::~EpollServer() { Close(); }

void EpollServer::SetClientCapacity(int max_clients) {
  epoll_.Init(max_clients);
}

bool EpollServer::ProcessNetworkEvents() {
  for (auto iter : clients()) {
    if (iter.second->NeedWrite()) {
      epoll_.Modify(iter.second->sock_fd(), EPOLLIN | EPOLLOUT);
    } else {
      epoll_.Modify(iter.second->sock_fd(), EPOLLIN);
    }
  }

  int ret = epoll_.Wait(1);
  if (ret < 0) {
    LOG(ERROR) << "EpollServer ";
    return false;
  } else if (ret == 0) {
    return true;
  }

  auto events = epoll_.events();
  for (int i = 0; i < ret; ++i) {
    Client* pclient = static_cast<Client*>(events[i].data.ptr);
    if (pclient) {
      if (events[i].events & EPOLLIN) {
        if (SOCKET_ERROR == ReceiveData(pclient)) {
          RemoveClient(pclient);
          continue;
        }
      }
      if (events[i].events & EPOLLOUT) {
        if (SOCKET_ERROR == pclient->SendDataReal()) {
          RemoveClient(pclient);
        }
      }
    }
  }
  return true;
}

void EpollServer::RemoveClient(Client* pclient) {
  auto iter = clients().find(pclient->sock_fd());
  if (iter != clients().end()) {
    clients().erase(iter);
  }
  OnClientLeave(pclient);
}

void EpollServer::OnClientJoin(Client* client) {
  epoll_.Register(client->sock_fd(), EPOLLIN, client);
}
