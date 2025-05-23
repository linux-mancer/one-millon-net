#include "echo_server.h"
#include "cell.h"
#include "config.hpp"
#include "message_header.h"

EchoServer::EchoServer() {
  send_back_ = Config::GetInstance().HasKey("send_back");
  send_full_ = Config::GetInstance().HasKey("send_full");
  check_msg_id_ = Config::GetInstance().HasKey("check_msg_id");
}

void EchoServer::OnClientConnected(Client* client) {
  TcpServer::OnClientConnected(client);
}

void EchoServer::OnClientDisconnected(Client* client) {
  TcpServer::OnClientDisconnected(client);
}

void EchoServer::OnMessageReceived(Server* server, Client* client,
                                   DataHeader* header) {
  TcpServer::OnMessageReceived(server, client, header);

  switch (header->cmd) {
    case Cmd::kLogin: {
      client->ResetHeartbeat();
      LoginRequest* login = static_cast<LoginRequest*>(header);
      if (check_msg_id_) {
        if (login->msg_id != client->recv_msg_id()) {
          LOG(ERROR) << "OnMessageReceived socket<" << client->socket_fd()
                     << "> recv_msg_id<" << login->msg_id;
        }
        client->recv_msg_id_next();
      }

      if (send_back_) {
        LoginResponse ret;
        ret.msg_id = client->send_msg_id();
        if (SOCKET_ERROR == client->SendData(&ret)) {
          if (send_full_) {
            LOG(ERROR) << "Scoket<" << client->socket_fd() << "> send full";
          }
        } else {
          client->send_msg_id_next();
        }
      }
      break;
    }
  }
}
