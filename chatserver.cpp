#include "chatserver.h"

ChatServer::ChatServer (int port) {
  this->fServer = new Server(port);
}

ChatServer::~ChatServer () {
  this->fServer->disconnect();
}

void ChatServer::listen() {
  while(this->fServer->acceptConnections()) {

  }
}
