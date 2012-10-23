#include "socket.h"
#include "server.h"
#include "chatserver.h"

ChatServer::ChatServer (int port) {
  fServer = new Server(port);
}

ChatServer::~ChatServer () {
  delete fServer;
}

void ChatServer::listen() {
  fServer->acceptConnections();
  fServer->readAll(printData);
}