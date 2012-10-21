#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include "utils.h"
#include "server.h"

class ChatServer {
  public:
    ChatServer(int port = DEFAULT_PORT);
    ~ChatServer();

    void listen();

  private:
    Server* fServer;
};

#endif