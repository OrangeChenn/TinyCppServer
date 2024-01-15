#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include "ThreadPool.h"

#include <vector>

EventLoop::EventLoop() { poller_ = std::make_unique<Epoll>(); }

EventLoop::~EventLoop() {}

void EventLoop::Loop() {
  while (true) {
    std::vector<Channel *> chs;
    chs = poller_->Poll();

    for (auto &it : chs) {
      //处理事件
      it->HandleEvent();
    }
  }
}

void EventLoop::UpdateChannel(Channel *channel) { poller_->UpdateChannel(channel); }

void EventLoop::DeleteChannel(Channel *channel) { poller_->DeleteChannel(channel); }
