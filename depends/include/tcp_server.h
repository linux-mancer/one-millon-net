#include <bits/stdint-uintn.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

#include "cell.h"
#include "server.h"
#include "client.h"
#include "network_event_listener.hpp"
#include "network.hpp"
#include "config.hpp"
#include "thread.hpp"
#include "timestamp.hpp"

class TcpServer : public NetworkEventListener {
 public:
  TcpServer();
  virtual ~TcpServer();
  SOCKET Init();
  int Bind(const char* ip, uint16_t port);
  int Listen(int n);
  SOCKET Accept();
  void AddClientToServer(Client* client_ptr);
  void Close();

  virtual void OnClientConnected(Client* client_ptr) override;
  virtual void OnClientDisconnected(Client* client_ptr) override;
  virtual void OnMessageReceived(Server* server, Client* client,
                                 DataHeader* header) override;
  virtual void OnDataReceived(Client* client) override;

  template <class ServerT>
  void Start(int server_count) {
    for (int n = 0; n < server_count; ++n) {
      auto ser = new ServerT();
      ser->SetId(n + 1);
      ser->SetClientCapacity(max_clients_ / server_count + 1);
      servers_.push_back(ser);
      ser->SetEventHandler(this);
      ser->Start();
    }
    thread_.Start(nullptr, [this](Thread* thread) { Run(thread); });
  }

 protected:
  virtual void Run(Thread* thread) = 0;
  void PrintMsg();
  SOCKET sockfd();

 private:
  Thread thread_;
  std::vector<Server*> servers_;
  // MilliTimer
  SOCKET sockfd_;

 protected:
  int send_buffer_size_;
  int recv_buffer_size_;
  int max_clients_;
  std::atomic_int recv_count_;
  std::atomic_int msg_count_;
  std::atomic_int client_accept_;
  std::atomic_int client_join_;
  MilliTimer timer_;
};
