#include <fcntl.h>
#include <cassert>

#include "Acceptor.h"
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"

Acceptor::Acceptor(EventLoop *loop) {
  sock_ = std::make_unique<Socket>();
  assert(sock_->Create() == RC_SUCCESS);
  assert(sock_->Bind("127.0.0.1", 8848) == RC_SUCCESS);
  assert(sock_->Listen() == RC_SUCCESS);

  channel_ = std::make_unique<Channel>(loop, sock_->Fd());
  std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
  channel_->SetReadCallback(cb);
  channel_->EnableReading();
}

Acceptor::~Acceptor() {}

RC Acceptor::AcceptConnection() {
  int clnt_fd = -1;
  if (sock_->Accept(&clnt_fd) != RC_SUCCESS) {
    return RC_ACCEPTOR_ERROR;
  }

  fcntl(clnt_fd, F_SETFL, fcntl(clnt_fd, F_GETFL) & O_NONBLOCK);
  if (new_connection_callback_) {
    new_connection_callback_(clnt_fd);
  }
  return RC_SUCCESS;
}

void Acceptor::SetNewConnectionCallback(std::function<void(int)> const &callback) {
  new_connection_callback_ = std::move(callback);
}
