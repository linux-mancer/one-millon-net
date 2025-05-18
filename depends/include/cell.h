/**
 * @file cell.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @date 2025-05-17-09-28
 * @author Nemausa
 *
 */
#ifndef CELL_H_
#define CELL_H_

#include <cstddef>
#include <stdio.h>

#ifdef _WIN32

#define FD_SETSIEZE 65535
#define WIN32_LEAN_AND_MEAN
#define _WINSCOK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#ifdef __APPLE__
#define _DARWIN_UNLIMITED_SELECT
#endif
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#endif

#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 8192
#define SEND_BUFF_SIZE 10240
#endif

#endif  // CELL_H_
