#pragma once

class TcpServer;
class EventLoop;
class Epoll;
class Acceptor;
class Connection;
class Channel;
class InetAddress;
class Socket;
class Buffer;
class ThreadPool;

#define DISALLOW_COPY(cname)                             \
  cname(const cname &) = delete;            /* NOLINT */ \
  cname &operator=(const cname &) = delete; /* NOLINT */

#define DISALLOW_MOVE(cname)                        \
  cname(cname &&) = delete;            /* NOLINT */ \
  cname &operator=(cname &&) = delete; /* NOLINT */

#define DISALLOW_COPY_AND_MOVE(cname) \
  DISALLOW_COPY(cname);               \
  DISALLOW_MOVE(cname);

// #define ASSERT(expr, message) assert((expr) && (message))

// #define UNREACHABLE(message) throw std::logic_error(message)

enum RC {
  RC_UNDEFINED = 0,
  RC_SUCCESS,
  RC_SOCKET_ERROR,
  RC_POLLER_ERROR,
  RC_CONNECTION_ERROR,
  RC_ACCEPTOR_ERROR,
  RC_UNIMPLEMENTED
};
