#pragma once
#include <arpa/inet.h>
#include "Common.h"

class InetAddress {
 public:
  InetAddress();
  InetAddress(const char *ip, uint16_t port);
  ~InetAddress() = default;

  void SetAddr(struct sockaddr_in addr);
  struct sockaddr_in GetAddr();
  const char *GetIp();
  uint16_t GetPort();

  DISALLOW_COPY_AND_MOVE(InetAddress);

 private:
  struct sockaddr_in addr_ {};
};
