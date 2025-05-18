/**
 * @file network_event_listener.hpp
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef NETWORK_EVENT_LISTENER_H_
#define NETWORK_EVENT_LISTENER_H_

#include "message_header.h"
class Server;
class Client;

class NetworkEventListener {
 public:
  virtual ~NetworkEventListener() = default;
  virtual void OnClientConnected(Client* client) = 0;
  virtual void OnClientDisconnected(Client* client) = 0;
  virtual void OnMessageReceived(Server* server, Client* client,
                                 DataHeader* header) = 0;
  virtual void OnDataReceived(Client* client) = 0;
};

#endif  // NETWORK_EVENT_LISTENER_H_
