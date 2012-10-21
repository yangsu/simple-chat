#include "client.h"

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
        debugf("ERROR, no such host\n");
        fReady = false;
    }
}

void Client::onFailedConnection(int sockfd) { //cleanup and recreate socket
    this->close(fSockfd);
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
            return conn;
        if (errno != EISCONN) {
            debugf("error: %s\n", strerror(errno));
            this->onFailedConnection(fSockfd);
            return conn;
        }
    }
    fConnected = true;
    debugf("Succesfully reached server\n");
    return 0;
}
