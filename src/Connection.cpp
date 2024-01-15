#include "Connection.h"

#include <sys/socket.h>
#include <sys/types.h>

#include <unistd.h>
#include <cassert>
#include <cstring>
#include <utility>

#include "Buffer.h"
#include "Channel.h"
#include "Common.h"
#include "EventLoop.h"
#include "Socket.h"
#include "util.h"

const int READ_BUFFER = 1024;

Connection::Connection(int fd, EventLoop *loop) {
  sock_ = std::make_unique<Socket>();
  sock_->SetFd(fd);
  if (loop != nullptr) {
    channel_ = std::make_unique<Channel>(loop, fd);
    channel_->EnableReading();
    channel_->UseET();
  }
  read_buffer_ = std::make_unique<Buffer>();
  send_buffer_ = std::make_unique<Buffer>();
  state_ = State::Connected;
}

Connection::~Connection() {}

void Connection::Read() {
  assert(state_ == State::Connected);
  read_buffer_->Clear();
  if (sock_->IsNonBlocking()) {
    ReadNonBlocking();
  } else {
    ReadBlocking();
  }
}

void Connection::Write() {
  assert(state_ == State::Connected);
  if (sock_->IsNonBlocking()) {
    WriteNonBlocking();
  } else {
    WriteBlocking();
  }
}

void Connection::ReadNonBlocking() {
  int sockfd = sock_->Fd();
  char buf[READ_BUFFER];
  while (true) {
    memset(&buf, 0, sizeof(buf));
    ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
      read_buffer_->Append(buf, bytes_read);
    } else if (bytes_read == -1 && errno == EINTR) {
      printf("continue reading.\n");
      continue;
    } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      break;
    } else if (bytes_read == 0) {
      printf("read EOF, client fd %d disconnected!\n", sockfd);
      state_ = State::Closed;
      break;
    } else {
      printf("Other error on client fd %d.\n", sockfd);
      state_ = State::Closed;
      break;
    }
  }
}

void Connection::WriteNonBlocking() {
  int sockfd = sock_->Fd();
  int data_size = send_buffer_->Size();
  int data_left = data_size;
  char buf[data_size];
  memcpy(buf, send_buffer_->ToStr(), data_size);
  while (data_left > 0) {
    int bytes_write = ::write(sockfd, buf + data_size - data_left, data_left);
    if (bytes_write == -1 && errno == EINTR) {
      printf("continue write.\n");
      continue;
    }
    if (bytes_write == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      break;
    }
    if (bytes_write == -1) {
      printf("other error on client fd %d.\n", sockfd);
      state_ = State::Closed;
      break;
    }
    data_left -= bytes_write;
  }
}

void Connection::ReadBlocking() {
  int sockfd = sock_->Fd();
  unsigned int rcv_size = 0;
  socklen_t len = sizeof(rcv_size);
  getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &len);
  char buf[rcv_size];
  ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
  if (bytes_read > 0) {
    read_buffer_->Append(buf, bytes_read);
  } else if (bytes_read == 0) {
    printf("read EOF, blocking client fd %d disconnected!\n", sockfd);
    state_ = State::Closed;
  } else if (bytes_read == -1) {
    printf("Other error on blocking client %d\n", sockfd);
    state_ = State::Closed;
  }
}

void Connection::WriteBlocking() {
  int sockfd = sock_->Fd();
  ssize_t bytes_write = ::write(sockfd, send_buffer_->ToStr(), send_buffer_->Size());
  if (bytes_write == -1) {
    printf("Other error on blocking client fd %d\n", sockfd);
    state_ = State::Closed;
  }
}

void Connection::Send(const std::string &msg) {
  SetSendBuffer(msg.c_str());
  Write();
}

void Connection::Business() {
  Read();
  on_recv_(this);
}

void Connection::Close() { delete_connection_callback_(sock_->Fd()); }

Connection::State Connection::GetState() { return state_; }

void Connection::SetSendBuffer(const char *str) { send_buffer_->SetBuf(str); }

Buffer *Connection::GetSendBuffer() { return send_buffer_.get(); }

const char *Connection::SendBuffer() { return send_buffer_->ToStr(); }

void Connection::GetlineSendBuffer() {}

Buffer *Connection::GetReadBuffer() { return read_buffer_.get(); }

const char *Connection::ReadBuffer() { return read_buffer_->ToStr(); }

Socket *Connection::GetSocket() { return sock_.get(); }

void Connection::SetDeleteConnectionCallback(std::function<void(int)> const &callback) {
  delete_connection_callback_ = std::move(callback);
}

void Connection::SetOnRecv(std::function<void(Connection *)> const &callback) {
  on_recv_ = std::move(callback);
  std::function<void()> bus = std::bind(&Connection::Business, this);
  channel_->SetReadCallback(bus);
}
