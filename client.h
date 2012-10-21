#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>
#include "utils.h"
#include "socket.h"

class Client : public Socket {
public:
    Client(const char* hostname, int port = DEFAULT_PORT);

    /**
     * Connect to server. Returns -1 on error or failure. Call this to connect
     * or reconnect to the server. For blocking sockets, this method will block
     * until the connection is accepted by the server.
     */
    int connectToServer();

protected:
    /**
     * Client needs to recreate the socket when a connection is broken because
     * connect can only be called successfully once.
     */
    virtual void onFailedConnection(int sockfd);

private:
    sockaddr_in fServerAddr;
};

#endif