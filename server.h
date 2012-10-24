#ifndef SERVER_H
#define SERVER_H

#include "socket.h";
#include "utils.h";

using namespace std;

class Server : public Socket {
  public:
    Server(int port = DEFAULT_PORT);
    ~Server();

    int readAll(void (*onRead)(int cid, header h, const void* data));
    int writeAll(header h, void* data);
    int acceptConnections();
};

#endif