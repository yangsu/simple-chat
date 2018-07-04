#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cerrno>
#include <cstring>
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
      if (errno == EINPROGRESS || errno == EALREADY) {
        debugf("in progress");
        fd_set rset, wset;
        FD_ZERO(&rset);
        FD_SET(fSockfd, &rset);
        wset = rset;
        timeval timeout;
        timeout.tv_sec = CONNECTION_TIMEOUT;
        timeout.tv_usec = 0;
        int sel = select(fSockfd + 1, &rset, &wset, NULL, &timeout);
        if (sel < 0) {
          debugf("select() failed with error %s", strerror(errno));
          return -1;
        } else if (sel == 0) {
          errno = ETIMEDOUT;
          return -1;
        }
      } else if (errno != EISCONN) {
        debugf("error: %s", strerror(errno));
        this->onFailedConnection(fSockfd);
        return conn;
      }
    }
    fConnected = true;
    debugf("Succesfully reached server");
    return 0;
}
