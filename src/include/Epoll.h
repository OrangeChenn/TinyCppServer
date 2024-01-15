#pragma once

#include <sys/epoll.h>
#include <vector>
#include "Common.h"

class Channel;
class Epoll {
 public:
  Epoll();
  ~Epoll();

  DISALLOW_COPY_AND_MOVE(Epoll);

  std::vector<Channel *> Poll(int timeout = -1);
  void UpdateChannel(Channel *channel);
  void DeleteChannel(Channel *channel);

 private:
  int epollfd_ = -1;
  struct epoll_event *events_ = nullptr;
};
