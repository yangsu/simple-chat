#ifndef SERVER_H
#define SERVER_H

#include <map>
#include "socket.h";
#include "utils.h";

using namespace std;

class Server : Socket {
  public:
    Server(int port = DEFAULT_PORT);
    ~Server();

  private:
    void disconnectAll();
    void acceptConnections();

    struct Info {
      string name;
      string address;
    };

    map<int, Info> map;
};

#endif