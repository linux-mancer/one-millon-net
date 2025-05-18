#include "epoll_tcp_server.h"
#include <sys/epoll.h>
#include "epoll_server.h"
#include "epoll_wrapper.h"
#include "thread.hpp"

void EpollTcpServer::Start(int server_count) {
  TcpServer::Start<EpollServer>(server_count);
}

void EpollTcpServer::Run(Thread* thread) {
  EpollWrapper epoll;
  epoll.Init(max_clients_);
  epoll.Register(sockfd(), EPOLLIN);
  while (thread->IsRunning()) {
    PrintMsg();
    int ret = epoll.Wait(1);
    if (ret < 0) {
      LOG(ERROR) << "EpollTcpServer Run wait exit";
      thread->Exit();
      break;
    }

    auto buffer = epoll.events();
    for (int i = 0; i < ret; ++i) {
      // 当服务器sockef发生事件时，表示有新的客户端连接
      if (buffer[i].data.fd == sockfd()) {
        if (buffer[i].events & EPOLLIN) {
          Accept();
        }
      }
    }
  }
}
