/**
 * @file server.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <chrono>
#include <thread>
#include <map>
#include "cell.h"
#include "message_header.h"
#include "network_event_listener.hpp"
#include "client.h"
#include "task_server.h"
#include "message_header.h"
#include "thread.hpp"
#include "semaphore.hpp"

class Server {
 public:
  virtual ~Server();
  void SetId(int id);
  void SetEventHandler(NetworkEventListener* handler);

  void Start();
  void Close();
  void AddClient(Client* client);
  size_t GetClientCount() const;

  virtual void SetClientCapacity(int max_clients);

 protected:
  virtual bool ProcessNetworkEvents() = 0;
  virtual void OnClientJoin(Client*) {}
  virtual void OnClientLeave(Client*) {}
  virtual void OnMessage(Client*, DataHeader*) {}
  int ReceiveData(Client* client);

 private:
  void Run(Thread* thread);

  void HandleNewClients_();
  void CheckHeartbeats_();
  void DispatchMessage_();
  void ClearAllClients_();
  std::map<SOCKET, Client*>& clients();

 private:
  std::map<SOCKET, Client*> clients_;
  std::vector<Client*> new_clients_;
  mutable std::mutex clients_mutex_;

  NetworkEventListener* event_handler_ = nullptr;
  TaskServer task_server_;
  Thread thread_;

  std::chrono::steady_clock::time_point last_check_time_ =
      std::chrono::steady_clock::now();
  int id_ = -1;
  int client_capacity_ = 0;
};

#endif  // SERVER_H_
