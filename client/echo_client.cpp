#include "echo_client.h"
#include "cell.h"
#include "config.hpp"
#include "message_header.h"

extern int msg_count;
extern int send_sleep;
extern int check_send_back;

EchoClient::EchoClient() {
  check_msg_id_ = Config::GetInstance().HasKey("check_msg_id");
}

void EchoClient::OnMessage(DataHeader* header) {
  send_ = false;
  switch (header->cmd) {
    case Cmd::kLoginResult: {
      LoginResponse* login = static_cast<LoginResponse*>(header);
      if (check_msg_id_) {
        if (login->msg_id != recv_msg_id_) {
          LOG(ERROR) << "OnMessage socket<" << client()->sock_fd() << ">msg id<"
                     << login->msg_id << ">recv_msg_id<" << recv_msg_id_ << "> "
                     << login->msg_id - recv_msg_id_;
        }
        client()->recv_msg_id_next();
      }

    } break;
  }
}

int EchoClient::SendTest(LoginRequest* login) {
  int ret = 0;
  if (send_count_ > 0 && !send_) {
    login->msg_id = send_msg_id_;
    ret = Send(login);
    if (SOCKET_ERROR != ret) {
      send_ = check_send_back;
      client()->send_msg_id_next();
      --send_count_;
    }
  }
  return ret;
}

bool EchoClient::CheckSend(time_t dt) {
  reset_time_ += dt;
  if (reset_time_ >= send_sleep) {
    reset_time_ -= send_sleep;
    send_count_ = msg_count;
  }
  return send_count_ > 0;
}

time_t EchoClient::reset_time() { return reset_time_; }

void EchoClient::set_reset_time(time_t t) { reset_time_ = t; }
