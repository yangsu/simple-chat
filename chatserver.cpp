#include "socket.h"
#include "server.h"
#include "chatserver.h"

ChatServer::ChatServer (int port) {
  fServer = new Server(port);
  fUniqueID = 0;
}

ChatServer::~ChatServer () {
  delete fServer;
}

void printData(int cli, header h, const void* data) {
  if (h.size) {
    string s = string((char*)data, h.size);
    debugf("read %u bytes from %d %s", h.size, cli, s.c_str());
  }
}

void ChatServer::listen() {
  int newfd = fServer->acceptConnections();
  if (newfd > 0) {
    char buffer[50];
    int id = fUniqueID++;
    int length = sprintf(buffer, "User %d", id);
    debugf("[%s] %d bytes", buffer, length);
    fServer->writeDataToFd(newfd, header(id, kLogin,length), (void*) buffer);
  }
  fServer->readAll(printData);
}