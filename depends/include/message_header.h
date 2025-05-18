/**
 * @file message_header.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @date 2025-05-17-08-34
 * @author Nemausa
 *
 */
#ifndef message_header_H_
#define message_header_H_

enum class Cmd : int {
  kLogin = 10,
  kLoginResult,
  kLogout,
  kLogoutResult,
  kNewUserJoin,
  kC2SHeart,
  kS2CHeart,
  kError,
};

struct DataHeader {
  uint16_t data_length;
  Cmd cmd;
  DataHeader() : data_length(sizeof(DataHeader)), cmd(Cmd::kError) {}
};

struct LoginRequest : public DataHeader {
  char user_name[32];
  char password[32];
  char data[28];
  int msg_id;

  LoginRequest() : DataHeader(), msg_id(0) {
    data_length = sizeof(LoginRequest);
    cmd = Cmd::kLogin;
  }
};

struct LoginResponse : public DataHeader {
  int result;
  char data[88];
  int msg_id;
  LoginResponse() : DataHeader(), result(0), msg_id(0) {
    data_length = sizeof(LoginResponse);
    cmd = Cmd::kLoginResult;
  }
};

struct LogoutRequest : public DataHeader {
  char user_name[32];
  LogoutRequest() : DataHeader() {
    data_length = sizeof(LogoutRequest);
    cmd = Cmd::kLogout;
  }
};

struct LogoutResponse : public DataHeader {
  int result;
  LogoutResponse() : DataHeader(), result(0) {
    data_length = sizeof(LogoutResponse);
    cmd = Cmd::kLogoutResult;
  }
};

struct NewUserJoinNotification : public DataHeader {
  int socket;

  NewUserJoinNotification() : DataHeader(), socket(0) {
    data_length = sizeof(NewUserJoinNotification);
    cmd = Cmd::kNewUserJoin;
  }
};

struct ClientHeartbeat : public DataHeader {
  ClientHeartbeat() : DataHeader() {
    data_length = sizeof(ClientHeartbeat);
    cmd = Cmd::kC2SHeart;
  }
};

struct ServerHeartbeat : public DataHeader {
  ServerHeartbeat() : DataHeader() {
    data_length = sizeof(ServerHeartbeat);
    cmd = Cmd::kS2CHeart;
  }
};

#endif  // message_header_H_
