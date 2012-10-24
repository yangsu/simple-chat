#include "client.h"
#include "server.h"
#include "chatclient.h"

ChatClient::ChatClient () {
  debugf("created client");
  fClient = NULL;
  fConnecting = false;
  fConnected = false;
  fTargetId = 0;

  fServer = new Server(randomPort());
}

ChatClient::~ChatClient () {
  this->disconnect();
  delete fServer;
}

bool ChatClient::connectToServer(string hostname, int port) {
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
      tempClient->setName((const char*)data, h.size);
      tempClient->setId(h.targetId);
      printf("Welcome %s! Your id is %d\n", (char*)data, tempClient->fId);

      char ipbuffer[50];
      int length = sprintf(ipbuffer, "%s:%d", tempClient->fClient->fIP.c_str(), tempClient->fClient->fPort);
      string ip(ipbuffer, length);

      header h(0, tempClient->fId, kLogin, ip.length());

      tempClient->fClient->writeData(h, (void*)ip.c_str());

    } else if (h.type == kClientListResponse) {
      vector<string> clients = split(string((char*)data, h.size), '|');
      printf("%d other clients are connected:\n", (int)clients.size());
      for (int i = 0; i < clients.size(); ++i) {
        printf("\t%s\n", clients[i].c_str());
      }
    } else if (h.type == kClientConnectRequest) {
      tempClient->fConnecting = true;
      printf("%s (Y/N)\n", (char*)data);
    } else if (h.type == kClientConnectReject) {
      tempClient->fConnecting = false;
      tempClient->fConnected = false;
      printf("%s\n", (char*)data);
    } else if (h.type == kClientConnectAccept) {
      tempClient->fConnecting = false;
      tempClient->fConnected = true;
      tempClient->fTargetId = h.sourceId;
      printf("%s. Type messages and press enter to send\n", (char*)data);
    } else if (h.type == kChatMessage) {
      printf("[User %d]:%s\n", h.sourceId, (char*)data);
    } else if (h.type == kClientDisconnect) {
      printf("%s\n", (char*)data);
    }
  }
}

void ChatClient::setName(const char* name, size_t size) {
  this->fName.assign(name, size);
}

void ChatClient::setId(int id) {
  this->fId = id;
}

void processP2PData(int cli, header h, const void* data) {
  if (h.size > 0) {
    if (h.type == kClientConnectRequest) {
      tempClient->fConnecting = true;
      printf("%s (Y/N)\n", (char*)data);
    } else if (h.type == kClientConnectReject) {
      tempClient->fConnecting = false;
      tempClient->fConnected = false;
      printf("%s\n", (char*)data);
    } else if (h.type == kClientConnectAccept) {
      tempClient->fConnecting = false;
      tempClient->fConnected = true;
      tempClient->fTargetId = h.sourceId;
      printf("%s. Type messages and press enter to send\n", (char*)data);
    } else if (h.type == kChatMessage) {
      printf("[User %d]:%s\n", h.sourceId, (char*)data);
    } else if (h.type == kClientDisconnect) {
      printf("%s\n", (char*)data);
    }
  }
}

void ChatClient::update() {
  tempClient = this;
  this->fClient->readData(processPacket);

  // this->fServer->acceptConnections();
  // this->fServer->readAll(processP2PData);
}

void ChatClient::getAvailableClients() {
  if (this->fClient != NULL) {
    this->fClient->writeData(header(0, 0, kClientListRequest, 7), (void*)"request");
  }
}

void ChatClient::connectToClient(int id) {
  if (this->fClient != NULL) {
    string msg(this->fName);
    msg += " would like to chat with you!";
    this->fClient->writeData(header(id, this->fId, kClientConnectRequest, msg.length()), (void*)msg.c_str());
  }
}

void ChatClient::disconnectFromClient() {
  if (this->fClient != NULL && this->fConnected) {
    string msg(this->fName);
    msg += " has stopped chatting with you!";
    this->fClient->writeData(header(this->fTargetId, this->fId, kClientDisconnect, msg.length()), (void*)msg.c_str());
    this->fConnected = false;
    this->fTargetId = 0;
  }
}

void ChatClient::acceptConnection(int id) {
  if (this->fClient != NULL) {
    string msg(this->fName);
    msg += " has accepted your request to chat!";
    this->fClient->writeData(header(id, this->fId, kClientConnectAccept, msg.length()), (void*)msg.c_str());
    this->fConnected = true;
    this->fTargetId = id;
  }
}

void ChatClient::rejectConnection(int id) {
  if (this->fClient != NULL) {
    string msg(this->fName);
    msg += " has rejected your request to chat!";
    this->fConnected = false;
    this->fClient->writeData(header(id, this->fId, kClientConnectReject, msg.length()), (void*)msg.c_str());
  }
}
void ChatClient::sendMessage(string msg) {
  if (this->fClient != NULL && this->fConnected) {
    header h(this->fTargetId, this->fId, kChatMessage, msg.length());
    this->fClient->writeData(h, (void*)msg.c_str());
  }
}