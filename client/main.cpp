#include <unistd.h>
#include <atomic>
#include <iostream>
#include <vector>
#include "cell.h"
#include "config.hpp"
#include "echo_client.h"
#include "message_header.h"
#include "thread.hpp"
#include "timestamp.hpp"

const char* sz_ip = "127.0.0.1";
uint16_t port = 4567;
int thread_count = 1;
int client_count = 1;
int work_sleep = 1;
int send_buffer_size = SEND_BUFF_SIZE;
int recv_buffer_size = RECV_BUFF_SIZE;
int msg_count = 1;
int send_sleep = 1;
int check_send_back = true;


std::atomic_int send_count(0);
std::atomic_int ready_count(0);
std::atomic_int connect_count(0);

void WorkThread(Thread* thread, int id) {
  LOG(INFO) << "thread<" << id << "> start";
  std::vector<EchoClient*> clients(client_count);
  int begin = 0;
  int end = client_count / thread_count;
  if (end < 1) end = 1;
  int temp = send_sleep > 0 ? send_sleep : 1;
  for (int n = begin; n < end; ++n) {
    if (!thread->IsRunning()) break;
    clients[n] = new EchoClient();
    clients[n]->set_reset_time(n % temp);
    Thread::Sleep(0);
  }

  for (int n = begin; n < end; ++n) {
    if (!thread->IsRunning()) break;
    if (INVALID_SOCKET ==
        clients[n]->CreateSocket(send_buffer_size, recv_buffer_size))
      break;
    if (SOCKET_ERROR == clients[n]->Connect(sz_ip, port)) break;
    connect_count++;
    Thread::Sleep(0);
  }

  LOG(INFO) << "thread<" << id << ">connect<begin=" << begin << " end=" << end
            << " connect=" << connect_count << ">";
  ready_count++;
  while (ready_count < thread_count && thread->IsRunning()) {
    Thread::Sleep(10);
  }

  LoginRequest login;
  strcpy(login.user_name, "lyd");
  strcpy(login.password, "lydmm");

  MilliTimer timer;
  auto t2 = timer.Elapsed();
  auto t0 = t2;
  auto dt = t0;
  while (thread->IsRunning()) {
    t0 = timer.Elapsed();
    dt = t0 - t2;
    t2 = t0;
    {
      int count = 0;
      for (int m = 0; m < msg_count; ++m) {
        for (int n = begin; n < end; ++n) {
          if (clients[n]->is_connected()) {
            if (clients[n]->SendTest(&login) > 0) {
              ++send_count;
            }
          }
        }
      }

      for (int n = begin; n < end; ++n) {
        if (clients[n]->is_connected()) {
          if (!clients[n]->Run(0)) {
            connect_count--;
            continue;
          }
          clients[n]->CheckSend(dt);
        }
      }
    }
    Thread::Sleep(work_sleep);
  }

  for (int n = begin; n < end; n++) {
    clients[n]->Close();
    delete clients[n];
  }

  LOG(INFO) << "thread<" << id << ">,exit";
  --ready_count;
}

int main(int argc, char* argv[]) {
  Config::GetInstance().Init(argc, argv);

  std::string ip_str = Config::GetInstance().GetString("ip", "127.0.0.1");
  const char* sz_ip = ip_str.c_str();
  port = Config::GetInstance().GetInt("port", 4567);
  thread_count = Config::GetInstance().GetInt("thread_count", 1);
  client_count = Config::GetInstance().GetInt("client_count", 10000);
  msg_count = Config::GetInstance().GetInt("msg_count", 10);
  send_sleep = Config::GetInstance().GetInt("send_sleep", 100);
  work_sleep = Config::GetInstance().GetInt("work_sleep", 1);
  check_send_back = Config::GetInstance().GetInt("check_send_back");
  send_buffer_size =
      Config::GetInstance().GetInt("send_buffer_size", SEND_BUFF_SIZE);
  recv_buffer_size =
      Config::GetInstance().GetInt("recv_buffer_size", RECV_BUFF_SIZE);

  Thread thread_cmd;
  thread_cmd.Start(nullptr, [](Thread* thread) {
    while (true) {
      char cmd[256] = {};
      scanf("%s", cmd);
      if (0 == strcmp(cmd, "exit")) {
        LOG(INFO) << "exit thread";
        break;
      } else {
        LOG(INFO) << "undefined cmd";
      }
    }
  });

  std::vector<Thread*> threads;
  for (int n = 0; n < thread_count; ++n) {
    Thread* t = new Thread();
    t->Start(nullptr, [n](Thread* thread) { WorkThread(thread, n + 1); });
  }

  MilliTimer timer;
  while (thread_cmd.IsRunning()) {
    auto t = timer.Elapsed() / 1000.0;
    if (t > 1.0) {
      LOG(INFO) << "thread<" << thread_count << ">, clients<" << client_count
                << ">,connect<" << connect_count << ">,send<" << send_count
                << ">";
      send_count = 0;
      timer.Reset();
    }
    Thread::Sleep(1);
  }

  for (auto t : threads) {
    t->Stop();
    delete t;
  }

  LOG(INFO) << "Exit";

  return 0;
}
