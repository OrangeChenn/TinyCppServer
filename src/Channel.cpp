#include "Channel.h"
#include "Epoll.h"
#include "EventLoop.h"

#include <unistd.h>

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd), events_(0), ready_(0), exits_(false) {}
Channel::~Channel() {
  if (fd_ != -1) {
    ::close(fd_);
    fd_ = -1;
  }
}

void Channel::EnableReading() {
  events_ |= EPOLLIN | EPOLLPRI;
  loop_->UpdateChannel(this);
}

void Channel::UseET() {
  events_ |= EPOLLIN | EPOLLET;
  loop_->UpdateChannel(this);
}

int Channel::GetFd() const { return fd_; }

int16_t Channel::GetEvents() const { return events_; }

int16_t Channel::GetReadyEvents() const { return ready_; }

void Channel::SetReadyEvents(int16_t _ev) { ready_ = _ev; }

void Channel::SetInEpoll(bool in /* = true */) { exits_ = in; }

bool Channel::GetInEpoll() { return exits_; }

void Channel::SetReadCallback(std::function<void()> const &callback) { read_callback_ = std::move(callback); }

void Channel::SetWriteCallback(std::function<void()> const &callback) { write_callback_ = std::move(callback); }

void Channel::HandleEvent() {
  if (ready_ & (EPOLLIN | EPOLLPRI)) {
    read_callback_();
  }

  if (ready_ & EPOLLOUT) {
    write_callback_();
  }
}
