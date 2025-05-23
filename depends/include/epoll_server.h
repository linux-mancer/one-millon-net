#pragma once

#include "client.h"
#include "server.h"
#include "epoll_wrapper.h"

class EpollServer : public Server {
 public:
  ~EpollServer() override;
  void SetClientCapacity(int max_clients) override;
  bool ProcessNetworkEvents() override;
  void RemoveClient(Client* pclient);

 protected:
  virtual void OnClientConnected(Client*) override;

 private:
  EpollWrapper epoll_;
};
