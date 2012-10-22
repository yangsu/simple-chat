#include "server.h"
#include "chatserver.h"

ChatServer::ChatServer (int port) {
  fServer = new Server(port);
}

ChatServer::~ChatServer () {
  delete fServer;
}

void readData(int cli, const void* data, size_t size) {
  string s = string((char*)data, size);
  debugf("%s", s.c_str());
}

void ChatServer::listen() {
  while(true) {
    fServer->acceptConnections();
    fServer->readData(readData);
  }
}