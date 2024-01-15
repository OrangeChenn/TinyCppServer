#pragma once

#include <functional>
#include <memory>
#include <string>
#include "Common.h"

class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection {
 public:
  enum State {
    Invalid = 1,
    Handshaking,
    Connected,
    Closed,
    Failed,
  };

  Connection(int fd, EventLoop *loop);
  ~Connection();

  DISALLOW_COPY_AND_MOVE(Connection);

  void Read();
  void Write();
  void Send(const std::string &msg);

  void SetDeleteConnectionCallback(std::function<void(int)> const &callback);
  void SetOnConnectCallback(std::function<void(Connection *)> const &callback);
  void SetOnRecv(std::function<void(Connection *)> const &callback);

  void Close();
  Connection::State GetState();
  void SetSendBuffer(const char *str);
  Buffer *GetSendBuffer();
  const char *SendBuffer();
  void GetlineSendBuffer();
  Buffer *GetReadBuffer();
  const char *ReadBuffer();
  Socket *GetSocket();

  void OnConnect(std::function<void()> fn);
  void OnMessage(std::function<void()> fn);

 private:
  std::unique_ptr<Socket> sock_;
  std::unique_ptr<Channel> channel_;

  State state_;
  std::unique_ptr<Buffer> read_buffer_;
  std::unique_ptr<Buffer> send_buffer_;

  std::function<void(int)> delete_connection_callback_;
  std::function<void(Connection *)> on_recv_;
  // std::function<void(Connection *)> on_connect_callback_;
  // std::function<void(Connection *)> on_message_callback_;

  void Business();
  void ReadNonBlocking();
  void WriteNonBlocking();
  void ReadBlocking();
  void WriteBlocking();
};
