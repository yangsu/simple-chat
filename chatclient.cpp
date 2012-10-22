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

void ChatClient::getAvailableClients() {
  // if (this->fClient != NULL) {
  this->fClient->writeData(3, (void*)"abc", 3);
  // }
}