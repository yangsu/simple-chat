#ifndef SERVER_H
#define SERVER_H

#include <map>
#include "utils.h";
#include "socket.h";

using namespace std;

class Server : Socket {
  public:
    Server(int port);
    ~Server();

  private:
    struct Info {
      string name;
      string address;
    };

    std::map<int, Info> map;
};

#endif