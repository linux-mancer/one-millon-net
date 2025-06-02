/**
 * @file tcp_client_manager.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 *
 */
#ifndef TCP_CLIENT_MANAGER_H_
#define TCP_CLIENT_MANAGER_H_

#if defined(_WIN32)
#elif defined(__linux__)
#include "epoll_tcp_client.h"
#else
#endif

#if defined(_WIN32)
using TcpClientManager = TcpIocpClient;
#elif defined(__linux__)
using TcpClientManager = TcpEpollClient;
#else
using TcpClientManager = TcpSelectClient;
#endif

#endif  // TCP_CLIENT_MANAGER_H_
