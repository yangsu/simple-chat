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
    string getClients(int clientFd);
    Server* fServer;

  private:
    struct ClientInfo {
      ClientInfo(int f, string n) {
        this->fd = f;
        this->name = n;
      }
      int fd;
      string name;
    };
    map<int, ClientInfo> fClientMap;
    int fUniqueID;
};

#endif