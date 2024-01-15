#pragma once

#include <functional>
#include <memory>
#include "Common.h"

class Epoll;
class Channel;
class ThreadPool;

class EventLoop {
 public:
  EventLoop();
  ~EventLoop();

  DISALLOW_COPY_AND_MOVE(EventLoop);

  void Loop();
  void UpdateChannel(Channel *channel);
  void DeleteChannel(Channel *channel);

 private:
  std::unique_ptr<Epoll> poller_;
};
