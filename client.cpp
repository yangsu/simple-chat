#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cerrno>

#include "client.h"

using namespace std;

Client::Client(const char* hostname, int port) {
    //Add fSockfd since the client will be using it to read/write
    this->addToMasterSet(fSockfd);

    hostent* server = gethostbyname(hostname);
    if (server) {
        fServerAddr.sin_family = AF_INET;
        memcpy((char*)&fServerAddr.sin_addr.s_addr, (char*)server->h_addr,
               server->h_length);
        fServerAddr.sin_port = htons(port);
    }
    else {
        debugf("ERROR, no such host");
        fReady = false;
    }
}

void Client::onFailedConnection(int sockfd) { //cleanup and recreate socket
    this->closeFd(fSockfd);
    fSockfd = this->create();
    //Add fSockfd since the client will be using it to read/write
    this->addToMasterSet(fSockfd);
}

int Client::connectToServer() {
    if (!fReady)
        return -1;
    if (fConnected)
        return 0;

    int conn = connect(fSockfd, (sockaddr*)&fServerAddr, sizeof(fServerAddr));
    if (conn < 0) {
        if (errno == EINPROGRESS || errno == EALREADY)
            debugf("error: %s", strerror(errno));
            // return conn;
        else if (errno != EISCONN) {
            debugf("error: %s", strerror(errno));
            this->onFailedConnection(fSockfd);
            return conn;
        }
    }
    fConnected = true;
    debugf("Succesfully reached server");
    return 0;
}
