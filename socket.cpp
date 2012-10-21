#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "Sockets.h"
#include "SkData.h"

Socket::Socket() {
  fMaxfd = 0;
  FD_ZERO(&fMasterSet);
  fConnected = false;
  fReady = false;
  fReadSuspended = false;
  fWriteSuspended = false;
  fSockfd = this->createSocket();
}

Socket::~Socket() {
  this->closeSocket(fSockfd);
  shutdown(fSockfd, 2);
}

int Socket::createSocket() {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    debugf("ERROR opening socket\n");
    return -1;
  }

  int reuse = 1;

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
    debugf("error: %s\n", strerror(errno));
    return -1;
  }

  // set non blocking
  int flags = fcntl(sockfd, F_GETFL);
  fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

  debugf("Opened fd:%d\n", sockfd);
  fReady = true;
  return sockfd;
}

void Socket::closeSocket(int sockfd) {
  if (!fReady)
    return;

  close(sockfd);
  debugf("Closed fd:%d\n", sockfd);

  if (FD_ISSET(sockfd, &fMasterSet)) {
    FD_CLR(sockfd, &fMasterSet);
    if (sockfd >= fMaxfd) {
      while (FD_ISSET(fMaxfd, &fMasterSet) == false && fMaxfd > 0)
        fMaxfd -= 1;
    }
  }
  if (0 == fMaxfd)
    fConnected = false;
}

void Socket::onFailedConnection(int sockfd) {
  this->closeSocket(sockfd);
}

void Socket::addToMasterSet(int sockfd) {
  FD_SET(sockfd, &fMasterSet);
  if (sockfd > fMaxfd)
    fMaxfd = sockfd;
}

int Socket::read(void (*onRead)(int, const void*, size_t)) {
  if (!fConnected || !fReady || NULL == onRead)
    return -1;

  int totalBytesRead = 0;

  return totalBytesRead;
}

int Socket::writePacket(void* data, size_t size) {
  if (size <= 0 || NULL == data || !fConnected || !fReady)
    return -1;

  int totalBytesWritten = 0;
  return totalBytesWritten;
}
