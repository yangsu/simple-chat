#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "utils.h"
#include "client.h"

class ChatClient {
  public:
    ChatClient();
    ~ChatClient();

    bool connectToServer(string hostname, int port);
    void disconnect();

    // void getAvailableClients();
    // void send();
    // void receive();

  private:
    Client* fClient;
};

#endif