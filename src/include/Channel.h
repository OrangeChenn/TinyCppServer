#pragma once

#include <sys/epoll.h>
#include <functional>
#include "Common.h"

class Epoll;
class EventLoop;

class Channel {
 public:
  Channel(EventLoop *loop, int fd);
  ~Channel();

  DISALLOW_COPY_AND_MOVE(Channel);

  int GetFd() const;
  void EnableReading();
  int16_t GetEvents() const;
  int16_t GetReadyEvents() const;
  void SetReadyEvents(int16_t ev);
  bool GetInEpoll();
  void SetInEpoll(bool in = true);
  void UseET();

  void SetReadCallback(std::function<void()> const &callback);
  void SetWriteCallback(std::function<void()> const &callback);
  void HandleEvent();
  void EnableWrite();

 private:
  EventLoop *loop_;
  int fd_;
  int16_t events_;
  int16_t ready_;
  bool exits_;
  std::function<void()> read_callback_;
  std::function<void()> write_callback_;
};
