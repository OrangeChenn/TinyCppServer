#pragma once

#include <functional>
#include <memory>

#include "Common.h"

class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor {
 public:
  explicit Acceptor(EventLoop *loop);
  ~Acceptor();
  RC AcceptConnection();
  void SetNewConnectionCallback(std::function<void(int)> const &callback);

  DISALLOW_COPY_AND_MOVE(Acceptor);

 private:
  std::unique_ptr<Socket> sock_;
  std::unique_ptr<Channel> channel_;
  std::function<void(int)> new_connection_callback_;
};
