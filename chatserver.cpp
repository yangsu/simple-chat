#include "socket.h"
#include "server.h"
#include "chatserver.h"

ChatServer::ChatServer (int port) {
  fServer = new Server(port);
}

ChatServer::~ChatServer () {
  delete fServer;
}

void readData(int cli, header h, const void* data) {
  string s = string((char*)data, h.size);
  if (h.size) {
    debugf("read %u bytes from %d %s", h.size, cli, s.c_str());
  }
}

void ChatServer::listen() {
  while(true) {
    fServer->acceptConnections();
    fServer->readAll(readData);
    sleep(POLLING_INTERVAL);
  }
}