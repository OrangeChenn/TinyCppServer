#include "Epoll.h"
#include <unistd.h>
#include <cstring>
#include "Channel.h"
#include "util.h"

const int MAX_EVENTS = 1024;

Epoll::Epoll() {
  epollfd_ = ::epoll_create1(0);
  ErrorIf(epollfd_ == -1, "epoll create error.");
  events_ = new epoll_event[MAX_EVENTS];
  memset(events_, 0, sizeof(*events_) * MAX_EVENTS);
}

Epoll::~Epoll() {
  if (epollfd_ != -1) {
    ::close(epollfd_);
    epollfd_ = -1;
  }
  delete[] events_;
}

std::vector<Channel *> Epoll::Poll(int timeout) {
  std::vector<Channel *> active_channels;
  int nfds = ::epoll_wait(epollfd_, events_, MAX_EVENTS, timeout);
  ErrorIf(nfds == -1, "epoll wait error.");
  for (int i = 0; i < nfds; i++) {
    Channel *channel = (Channel *)events_[i].data.ptr;
    channel->SetReadyEvents(events_[i].events);
    active_channels.push_back(channel);
  }
  return active_channels;
}

void Epoll::UpdateChannel(Channel *channel) {
  int fd = channel->GetFd();
  struct epoll_event ev {};
  memset(&ev, 0, sizeof(ev));
  ev.data.ptr = channel;
  ev.events = channel->GetEvents();
  if (!channel->GetInEpoll()) {
    ErrorIf(::epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll ctl error.");
    channel->SetInEpoll();
  } else {
    ErrorIf(::epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll ctl error.");
  }
}

void Epoll::DeleteChannel(Channel *channel) {
  int fd = channel->GetFd();
  ErrorIf(epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, nullptr) == -1, "epoll delete error.");
  channel->SetInEpoll(false);
}
