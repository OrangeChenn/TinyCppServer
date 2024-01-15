#pragma once

#include <string>
#include "Common.h"

class Socket {
 public:
  DISALLOW_COPY_AND_MOVE(Socket);
  Socket();
  ~Socket();
  RC Create();
  RC Bind(const char *ip, uint16_t port) const;
  RC Listen() const;
  RC Accept(int *clnt_fd) const;
  RC Connect(const char *ip, uint16_t port) const;
  RC SetNonBlocking() const;
  bool IsNonBlocking() const;
  size_t RecvBufSize() const;
  void SetFd(int fd);
  int Fd() const;
  std::string GetAddr() const;

 private:
  int fd_;
};
