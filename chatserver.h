#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <map>
#include "utils.h"
#include "server.h"

class ChatServer {
  public:
    ChatServer(int port = DEFAULT_PORT);
    ~ChatServer();

    void listen();

    int getClientFd(int id);
    string getClientName(int id);

    string getClients(int clientFd);
    Server* fServer;

    map<int, ClientInfo*> fClientMap;
    int fUniqueID;
};

#endif