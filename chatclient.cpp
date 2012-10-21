#include "client.h"
#include "chatclient.h"

ChatClient::ChatClient () {
}

ChatClient::~ChatClient () {
}

bool ChatClient::connectToServer(const char* hostname, int port) {
  this->disconnect();
  this->fClient = new Client(hostname, port);
  return this->fClient->connectToServer();
}

void ChatClient::disconnect() {
  if (this->fClient) {
    this->fClient->closeAll();
    delete this->fClient;
  }
}