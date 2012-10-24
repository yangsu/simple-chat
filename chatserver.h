#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <map>
#include "utils.h"
#include "server.h"

class ChatServer {
  public:
    ChatServer(int port = DEFAULT_PORT);
    ~ChatServer();

    struct ClientInfo {
      ClientInfo(int f, string n) {
        this->fd = f;
        this->name = n;
      }
      int fd;
      string name;
    };

    void listen();

    int getClientFd(int id);
    string getClientName(int id);

    string getClients(int clientFd);
    Server* fServer;

  private:

    map<int, ClientInfo*> fClientMap;
    int fUniqueID;
};

#endif