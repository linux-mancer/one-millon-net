#include "server.h"
#include <glog/logging.h>
#include <chrono>
#include <mutex>

Server::~Server() {
  LOG(INFO) << "Server exit " << id_ << " begin";
  Close();
  LOG(INFO) << "Server exit " << id_ << " end";
}

void Server::SetId(int id) {
  id_ = id;
  task_server_.set_server_id(id);
}

void Server::SetClientCapacity(int max_clients) {
  client_capacity_ = max_clients;
}

void Server::SetEventHandler(NetworkEventListener* handler) {
  event_handler_ = handler;
}

void Server::Start() {
  task_server_.Start();
  thread_.Start(
      nullptr, [this](Thread* t) { Run(t); },
      [this](Thread*) { ClearAllClients_(); });
}

void Server::Close() {
  LOG(INFO) << "Server " << id_ << " Close start";
  task_server_.Stop();
  thread_.Stop();
  ClearAllClients_();
  LOG(INFO) << "Server " << id_ << " Close end";
}

void Server::AddClient(Client* client) {
  std::lock_guard<std::mutex> lk(clients_mutex_);
  new_clients_.push_back(client);
}

size_t Server::GetClientCount() const {
  std::lock_guard<std::mutex> lk(clients_mutex_);
  return clients_.size() + new_clients_.size();
}

int Server::ReceiveData(Client* client) {
  int len = client->RecvData();
  if (event_handler_) {
    event_handler_->OnDataReceived(client);
  }
  return len;
}

void Server::Run(Thread* thread) {
  while (thread->IsRunning()) {
    HandleNewClients_();

    if (clients_.empty()) {
      Thread::Sleep(1);
      last_check_time_ = std::chrono::steady_clock::now();
      continue;
    }

    CheckHeartbeats_();
    if (!ProcessNetworkEvents()) {
      thread->Stop();
      break;
    }
    DispatchMessage_();
  }
  LOG(INFO) << "Server " << id_ << " exit";
}

void Server::HandleNewClients_() {
  std::lock_guard<std::mutex> lk(clients_mutex_);
  for (Client* c : new_clients_) {
    clients_[c->sock_fd()] =c;
    if (event_handler_) {
      event_handler_->OnClientConnected(c);
    }
    OnClientJoin(c);
  }
  new_clients_.clear();
}

void Server::CheckHeartbeats_() {
  auto now = std::chrono::steady_clock::now();
  auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - last_check_time_)
                .count();
  last_check_time_ = now;
  for (auto it = clients_.begin(); it != clients_.end();) {
    Client* c = it->second;
    if (c->CheckHeart(dt)) {
      // if (event_handler_) {
      //   event_handler_->OnClientDisconnected(c);
      // }
      OnClientLeave(c);
      // delete c;
      it = clients_.erase(it);
      continue;
    } 
      it++;
  }
}

void Server::DispatchMessage_() {
  for (auto& kv : clients_) {
    Client* c = kv.second;
    while (c->HasMsg()) {
      DataHeader* header = c->FrontMsg();
      if (event_handler_) {
        event_handler_->OnMessageReceived(this, c, header);
      }
      OnMessage(c, header);
      c->PopFrontMsg();
    }
  }
}

void Server::ClearAllClients_() {
  for (auto& kv : clients_) {
    delete kv.second;
  }
  clients_.clear();
  for (Client* c : new_clients_) {
    delete c;
  }
  new_clients_.clear();
}

std::map<SOCKET, Client*>& Server::clients() {
  std::lock_guard<std::mutex> lock(clients_mutex_);
  return clients_;
}
