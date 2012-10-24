#include <sstream>
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

ChatServer* tempServer;

void processServerData(int cli, header h, const void* data) {
  if (h.size > 0) {
    if (h.type == kClientListRequest) {
      string clients = tempServer->getClients(cli);
      tempServer->fServer->writeDataToFd(cli, header(0, kClientListResponse, clients.length()), (void*) clients.c_str());
    }
  }
}

string ChatServer::getClients(int clientFd) {
  stringstream clients;

  map<int, ClientInfo>::iterator it;
  for ( it = fClientMap.begin() ; it != fClientMap.end(); it++ ) {
    if ((*it).second.fd != clientFd) {
      if (it != fClientMap.begin()) {
        clients << "|";
      }
      clients << (*it).second.name.c_str();
    }
  }

  return clients.str();
}

void ChatServer::listen() {
  int newfd = fServer->acceptConnections();
  if (newfd > 0) {
    char namebuffer[50];
    int id = fUniqueID++;
    int length = sprintf(namebuffer, "User %d", id);
    debugf("[%s] %d bytes", namebuffer, length);

    fClientMap.insert(pair<int, ClientInfo>(id, ClientInfo(newfd, string(namebuffer, length))));
    fServer->writeDataToFd(newfd, header(id, kLogin,length), (void*) namebuffer);
  }

  tempServer = this;
  fServer->readAll(processServerData);
}