/**
* @file tcp_server_manager.h
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @author Nemausa
*
*/
#ifndef TCP_SERVER_MANAGER_H_
#define TCP_SERVER_MANAGER_H_

#if defined(_WIN32)
#elif defined(__linux__)
#include "epoll_tcp_server.h"
#else
#endif

#if defined(_WIN32)
using tcp_server_manager = TcpIocpClient;
#elif defined(__linux__)
using tcp_server_manager = TcpEpollClient;
#else
using tcp_server_manager = TcpSelectClient;
#endif

#endif  // TCP_SERVER_MANAGER_H_
