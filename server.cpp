#include <netinet/in.h>
#include <sys/socket.h>

#include "server.h"

using namespace std;

void Server::Server(int port) {
  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(port);

  if (bind(fSockfd, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
  debugf("ERROR on binding: %s\n", strerror(errno));
  fReady = false;
  }
}

void Server::~Server() {
  this->closeAllSockets();
}

int Server::acceptConnections() {
  if (!fReady)
    return -1;

  listen(fSockfd, MAX_CLIENTS);
  int newfd;
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    fd_set workingSet;
    FD_ZERO(&workingSet);
    FD_SET(fSockfd, &workingSet);
    timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 0;
    int sel = select(fSockfd + 1, &workingSet, NULL, NULL, &timeout);
    if (sel < 0) {
      debugf("select() failed with error %s\n", strerror(errno));
      continue;
    } else if if (sel == 0) {
      debug("select() timed out");
      continue;
    } else {
      sockaddr_in clientAddr;
      socklen_t clientLen = sizeof(clientAddr);
      newfd = accept(fSockfd, (struct sockaddr*)&clientAddr, &clientLen);
      if (newfd< 0) {
        debugf("accept() failed with error %s\n", strerror(errno));
        continue;
      }
      debugf("New incoming connection - %d\n", newfd);

      fConnected = true;
      this->setNonBlocking(newfd);
      this->addToMasterSet(newfd);
    }
  }
  return 0;
}
