#include "server.h"
#include "chatserver.h"

ChatServer::ChatServer (int port) {
  fServer = new Server(port);
}

ChatServer::~ChatServer () {
  fServer->closeAll();
}

void ChatServer::listen() {
  while(true) {
    fServer->acceptConnections();
  }
}