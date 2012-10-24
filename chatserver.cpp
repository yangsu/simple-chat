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

  map<int, ClientInfo*>::iterator it;
  for ( it = fClientMap.begin() ; it != fClientMap.end(); it++ ) {
    delete (*it).second;
  }
  fClientMap.clear();
}

ChatServer* tempServer;

void processServerData(int cli, header h, const void* data) {
  if (h.size > 0) {
    if (h.type == kLogin) {
      vector<string> ipdata = split(string((char*)data, h.size), ':');
      ClientInfo* i = tempServer->fClientMap[h.sourceId];
      printf("Logged in: %s:%s\n", ipdata[0].c_str(), ipdata[1].c_str());
      i->ip = ipdata[0];
      i->port = atoi(ipdata[1].c_str());
    } else if (h.type == kP2PIPRequest) {
      string ip = tempServer->getClientIPNPort(h.targetId);
      header h2(h.sourceId, h.targetId, kP2PIPResponse, ip.length());
      tempServer->fServer->writeDataToFd(cli, h2, (void*) ip.c_str());
    } else if (h.type == kClientListRequest) {
      string clients = tempServer->getClients(cli);
      header h2(0, 0, kClientListResponse, clients.length());
      tempServer->fServer->writeDataToFd(cli, h2, (void*) clients.c_str());
    } else {
      int targetFd = tempServer->getClientFd(h.targetId);
      tempServer->fServer->writeDataToFd(targetFd, h, (void*) data);
    }
  }
}

int ChatServer::getClientFd(int id) {
  return this->fClientMap[id]->fd;
}
string ChatServer::getClientName(int id) {
  return this->fClientMap[id]->name;
}
string ChatServer::getClientIPNPort(int id) {
  return ipstring(fClientMap[id]->ip, fClientMap[id]->port);
}

string ChatServer::getClients(int clientFd) {
  stringstream clients;

  map<int, ClientInfo*>::iterator it;
  for ( it = fClientMap.begin() ; it != fClientMap.end(); it++ ) {
    if ((*it).second->fd != clientFd) {
      if (it != fClientMap.begin()) {
        clients << "|";
      }
      clients << (*it).second->name.c_str();
    }
  }

  return clients.str();
}

void ChatServer::listen() {
  int newfd = fServer->acceptConnections();
  if (newfd > 0) {
    char namebuffer[50];
    int id = ++fUniqueID;
    int length = sprintf(namebuffer, "User %d", id);

    ClientInfo* i = new ClientInfo(newfd, string(namebuffer, length));
    fClientMap.insert(pair<int, ClientInfo*>(id, i));
    fServer->writeDataToFd(newfd, header(id, 0, kLogin, length), (void*) namebuffer);

    printf("User %d has connected\n", id);
  }

  tempServer = this;
  fServer->readAll(processServerData);
}