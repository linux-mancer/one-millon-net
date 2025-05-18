#include <bits/stdint-uintn.h>
#include <glog/logging.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>
#include <string>
#include "echo_server.h"
#include "config.hpp"

int main(int argc, char* argv[]) {
  Config::GetInstance().Init(argc, argv);
  std::string ip_str = Config::GetInstance().GetString("ip", "127.0.0.1");
  const char* sz_ip = ip_str.c_str();
  uint16_t port = Config::GetInstance().GetInt("port", 4567);
  int thread_count = Config::GetInstance().GetInt("thread_count", 1);
  if (Config::GetInstance().HasKey("-p")) {
    LOG(INFO) << "HasKey -p";
  }

  if (strcmp(sz_ip, "any") == 0) {
    sz_ip = nullptr;
  }

  EchoServer server;
  server.Init();
  server.Bind(sz_ip, port);
  server.Listen(SOMAXCONN);
  server.Start(thread_count);

  while (true) {
    char cmd_buf[256] = {};
    scanf("%s", cmd_buf);
    if (0 == strcmp(cmd_buf, "exit")) {
      server.Close();
      break;
    } else {
      LOG(INFO) << "undefine cmd";
    }
  }

  LOG(INFO) << "exit";
  return 0;
}
