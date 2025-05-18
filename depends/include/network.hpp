/**
 * @file network.hpp
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "cell.h"

#ifdef _WIN32
#include <windsock2.h>
#else
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#endif

constexpr int kOn = 1;
#ifdef _WIN32
constexpr WORD kWinSockVersion = MAKEWORD(2, 2);
#endif

class Network {
 public:
  static void Init() {
#ifdef _WIN32
    WSADATA was_data;
    WSAStartup(kWinSockVersion, &wsa_data);
#else
    signal(SIGPIPE, SIG_IGN);
#endif
  }

  static bool MakeNonBlocking(SOCKET fd) noexcept {
#ifdef _WIN32
    unsigned long mode = kOn;
    if (ioctlsocek(fd, FIONBIN, &mode) == SOCKET_ERROR) {
      LOG(WARNING) << "ioctlsocek failed for" << fd;
      return false;
    }
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0 || (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)) {
      LOG(WARNING) << "fcntl non-blocking failed for " << fd;
      return false;
    }
#endif
    return true;
  }

  static bool MakeReuseAddr(SOCKET fd) noexcept {
    return SetSockOpt(fd, SOL_SOCKET, SO_REUSEADDR);
  }
  static bool MakeNoDelay(SOCKET fd) noexcept {
    return SetSockOpt(fd, IPPROTO_TCP, TCP_NODELAY);
  }

  static void DestroySocket(SOCKET fd) noexcept {
#if _WIN32
    closesocked(fd);
#else
    close(fd);
#endif  // _WIN32
  }

 private:
  static bool SetSockOpt(SOCKET fd, int level, int optname) noexcept {
    if (setsockopt(fd, level, optname, reinterpret_cast<const char*>(&kOn),
                   sizeof(kOn)) == SOCKET_ERROR) {
      LOG(WARNING) << "setsockopt " << fd << " opt " << optname << " failed";
      return false;
    }
    return true;
  }
};

#endif  // NETWORK_H_
