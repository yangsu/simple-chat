#ifndef SERVER_H
#define SERVER_H

#include <map>
#include "socket.h";
#include "utils.h";

using namespace std;

class Server : public Socket {
  public:
    Server(int port = DEFAULT_PORT);
    ~Server();

    int acceptConnections();

  private:

    struct Info {
      string name;
      string address;
    };

    map<int, Info> map;
};

#endif