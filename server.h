#ifndef SERVER_H
#define SERVER_H

#include <map>

using namespace std;

class Server {
  public:
    Server(int port);
    ~Server();

  private:
    int port;
    map<string, string> clients;
};

#endif