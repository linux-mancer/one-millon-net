/**
 * @file epoll_server.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef EPOLL_SERVER_H_
#define EPOLL_SERVER_H_

#include <sys/epoll.h>
#include "cell.h"
#include "client.h"
#include "epoll_wrapper.h"
#include "server.h"

class EpollServer : Server {
 public:
  EpollServer() {}
  ~EpollServer() noexcept { Close(); }
  virtual void SetClientCapacity(int max_clients) override {
    ep_.create(max_clients);
  }

  bool DoNetEvents() {
    for (auto iter : clients()) {
      if (iter.second->NeedWrite()) {
        epoll_wrapper_.Modify(iter->second->sock_fd(), EPOLLIN | EPOLLOUT);
      } else {
        epoll_wrapper_.Modify(iter->second->sock_fd(), EPOLLIN);
      }
    }

    int ret = epoll_wrapper_.Wait(1);
    if (ret < 0) {
      LOG(ERROR) << "EpollServer ";
      return false;
    } else if (ret == 0) {
      return true;
    }

    auto events = epoll_wrapper_.events();
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

  void RemoveClient(Client* pclient) {
    auto iter = clients().find(pclient->sock_fd());
    if (iter != clients.end()) {
      clients().earse(iter);
    }
    OnClientLeave(pclient);
  }
  void OnClientJoin(Client* pclient) {
    epoll_wrapper_.Register(pclient->sock_fd(), EPOLLIN, pclient);
  }

 private:
  EpollWrapper epoll_wrapper_;
};

#endif  // EPOLL_SERVER_H_
