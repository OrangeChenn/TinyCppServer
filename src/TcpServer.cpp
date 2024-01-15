#include "TcpServer.h"

#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <functional>

#include "Acceptor.h"
#include "Channel.h"
#include "Connection.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "util.h"

TcpServer::TcpServer() {
  main_reactor_ = std::make_unique<EventLoop>();
  acceptor_ = std::make_unique<Acceptor>(main_reactor_.get());
  std::function<void(int)> cb = std::bind(&TcpServer::NewConnection, this, std::placeholders::_1);
  acceptor_->SetNewConnectionCallback(cb);

  unsigned int size = std::thread::hardware_concurrency();
  thread_pool_ = std::make_unique<ThreadPool>(size);

  for (size_t i = 0; i < size; ++i) {
    std::unique_ptr<EventLoop> sub_reactor = std::make_unique<EventLoop>();
    sub_reactors_.push_back(std::move(sub_reactor));
  }
}

TcpServer::~TcpServer() {}

void TcpServer::Start() {
  for (size_t i = 0; i < sub_reactors_.size(); ++i) {
    std::function<void()> sub_loop = std::bind(&EventLoop::Loop, sub_reactors_[i].get());
    thread_pool_->AddTask(sub_loop);
  }
  main_reactor_->Loop();
}

RC TcpServer::NewConnection(int fd) {
  assert(fd != -1);
  int random = fd % sub_reactors_.size();
  std::unique_ptr<Connection> conn = std::make_unique<Connection>(fd, sub_reactors_[random].get());
  std::function<void(int)> cb = std::bind(&TcpServer::DeleteConnection, this, std::placeholders::_1);
  conn->SetDeleteConnectionCallback(cb);
  conn->SetOnRecv(on_recv_);
  connections_[fd] = std::move(conn);
  if (on_connect_) {
    on_connect_(connections_[fd].get());
  }
  return RC_SUCCESS;
}

RC TcpServer::DeleteConnection(int fd) {
  auto iter = connections_.find(fd);
  assert(iter != connections_.end());
  connections_.erase(iter);
  return RC_SUCCESS;
}

void TcpServer::OnConnect(std::function<void(Connection *)> const &fn) { on_connect_ = std::move(fn); }

void TcpServer::OnRecv(std::function<void(Connection *)> const &fn) { on_recv_ = std::move(fn); }
