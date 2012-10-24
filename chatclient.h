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
    int fTargetId;
    bool fConnecting;
    bool fConnected;

    bool connectToServer(string hostname, int port);
    void disconnect();

    void getAvailableClients();
    void connectToClient(int id);
    void disconnectFromClient();
    void acceptConnection(int id);
    void rejectConnection(int id);

    void sendMessage(string msg);

    void read();

    void setName(const char* name, size_t size);
    void setId(int id);

  private:
    Client* fClient;
};

#endif