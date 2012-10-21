#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "utils.h"
#include "client.h"

class ChatClient {
  public:
    ChatClient();
    ~ChatClient();

    bool connectToServer(const char* hostname, int port = DEFAULT_PORT);
    void disconnect();

  private:
    Client fClient;
};

#endif