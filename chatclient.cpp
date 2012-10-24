#include "client.h"
#include "chatclient.h"

ChatClient::ChatClient () {
  debugf("created client");
  this->fClient = NULL;
}

ChatClient::~ChatClient () {
  this->disconnect();
}

bool ChatClient::connectToServer(string hostname, int port) {
  printf("%s\n", hostname.c_str());
  this->disconnect();
  this->fClient = new Client(hostname.c_str(), port);
  if (this->fClient->connectToServer() == 0) {
    debugf("connected %s at port %d", hostname.c_str(), port);
    return true;
  }
  return false;
}

void ChatClient::disconnect() {
  if (this->fClient != NULL) {
    debugf("disconnecting client");
    this->fClient->closeAll();
    delete this->fClient;
  }
}

ChatClient* tempClient;

void processPacket(int cli, header h, const void* data) {
  if (h.size > 0) {
    if (h.type == kLogin) {
      // tempClient->setName((const char*)data, h.size);
      tempClient->setId(h.targetId);
      debugf("Welcome %s! Your id is %d", (char*)data, tempClient->fId);
    }
  }
}

void ChatClient::setName(const char* name, size_t size) {
  this->fName.assign(name, size);
}

void ChatClient::setId(int id) {
  this->fId = id;
}

void ChatClient::read() {
  tempClient = this;
  this->fClient->readData(processPacket);
}

void ChatClient::getAvailableClients() {
  // if (this->fClient != NULL) {
  this->fClient->writeData(header(0, kClientList, 3), (void*)"abc");
  // }
}