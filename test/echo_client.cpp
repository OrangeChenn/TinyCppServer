#include <iostream>
#include "Connection.h"
#include "Socket.h"

int main() {
  Socket *sock = new Socket();
  sock->Create();
  sock->Connect("127.0.0.1", 1234);

  Connection *conn = new Connection(sock->Fd(), nullptr);

  while (true) {
    std::string input;
    std::getline(std::cin, input);
    conn->SetSendBuffer(input.c_str());
    conn->Write();
    if (conn->GetState() == Connection::State::Closed) {
      conn->Close();
      break;
    }
    conn->Read();
    std::cout << "Message from server: " << conn->ReadBuffer() << std::endl;
  }

  delete conn;
  delete sock;
  return 0;
}
