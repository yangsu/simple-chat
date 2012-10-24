#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <string>
#include "utils.h"
#include "client.h"

class ChatClient {
  public:
    ChatClient();
    ~ChatClient();

    string fName;
    int fId;

    bool connectToServer(string hostname, int port);
    void disconnect();

    void getAvailableClients();
    void connectToClient(int id);
    void read();
    // void receive();
    void setName(const char* name, size_t size);
    void setId(int id);

  private:
    Client* fClient;
};

#endif