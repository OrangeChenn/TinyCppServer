#include <iostream>

#include "Connection.h"
#include "SignalHandler.h"
#include "Socket.h"
#include "TcpServer.h"

int main() {
  TcpServer *server = new TcpServer();

  Signal::SignalHandle(SIGINT, [&] {
    delete server;
    std::cout << "\nServer exit!" << std::endl;
  });

  server->OnConnect(
      [](Connection *conn) { std::cout << "New connection fd: " << conn->GetSocket()->Fd() << std::endl; });

  server->OnRecv([](Connection *conn) {
    std::cout << "Message from client " << conn->ReadBuffer() << std::endl;
    conn->Send(conn->ReadBuffer());
  });

  // server->OnMessage([](Connection *conn) {
  //   std::cout << "Message from client: " << conn->ReadBuffer() << std::endl;
  //   if (conn->GetState() == Connection::State::Connected) {
  //     conn->Send(conn->ReadBuffer());
  //   }
  // });

  // server->OnConnect([](Connection *conn) {
  //   conn->Read();
  //   if (conn->GetState() == Connection::State::Closed) {
  //     conn->Close();
  //     return;
  //   }
  //   std::cout << "Message from client " << conn->GetSocket()->GetFd() << ":" << conn->ReadBuffer() << std::endl;
  //   conn->SetSendBuffer(conn->ReadBuffer());
  //   conn->Write();
  // });
  server->Start();

  delete server;
  server = nullptr;
  return 0;
}
