#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <algorithm>
#include "utils.h"
#include "socket.h"

Socket::Socket() {
  fMaxfd = 0;
  FD_ZERO(&fMasterSet);
  fConnected = false;
  fReady = false;
  fSockfd = this->create();
}

Socket::~Socket() {
  this->closeFd(fSockfd);
  shutdown(fSockfd, 2);
}

int Socket::create() {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    debugf("ERROR opening socket");
    return -1;
  }

  int reuse = 1;

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
    debugf("error: %s", strerror(errno));
    return -1;
  }

  this->setNonBlocking(sockfd);

  debugf("Opened fd:%d", sockfd);
  fReady = true;
  return sockfd;
}

void Socket::closeFd(int sockfd) {
  if (!fReady)
    return;

  close(sockfd);
  debugf("Closed fd:%d", sockfd);

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

int Socket::closeAll() {
  for (int i = 0; i <= fMaxfd; ++i) {
    if (FD_ISSET(i, &fMasterSet))
      this->closeFd(i);
  }
  fConnected = false;
  return 0;
}

void Socket::onFailedConnection(int sockfd) {
  this->closeFd(sockfd);
}

void Socket::setNonBlocking(int sockfd) {
  int flags = fcntl(sockfd, F_GETFL);
  fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

void Socket::addToMasterSet(int sockfd) {
  FD_SET(sockfd, &fMasterSet);
  if (sockfd > fMaxfd)
    fMaxfd = sockfd;
}

int Socket::readData(void (*onRead)(int, const void*, size_t)) {
  if (!fConnected || !fReady || NULL == onRead)
    return -1;

  int totalBytesRead = 0;

  char buffer[MAX_PACKET_LENGTH];
  for (int i = 0; i <= fMaxfd; ++i) {
   if (!FD_ISSET (i, &fMasterSet))
     continue;

   memset(buffer, 0, MAX_PACKET_LENGTH);

   int attempts = 0;
   bool failure = false;

   int bytesReadInPacket = 0;

   while (fConnected && !failure) {
     int retval = read(i, buffer + bytesReadInPacket,
               MAX_PACKET_LENGTH - bytesReadInPacket);

     ++attempts;

     if (retval < 0) {
       if (errno == EWOULDBLOCK || errno == EAGAIN) {
         if (bytesReadInPacket > 0)
           continue; //incomplete buffer or frame, keep tring
         else
           break; //nothing to read
       }
       debugf("Read() failed with error: %s", strerror(errno));
       failure = true;
       break;
     }

     if (retval == 0) {
       debugf("Peer closed connection or connection failed");
       failure = true;
       break;
     }

     bytesReadInPacket += retval;
     if (bytesReadInPacket < MAX_PACKET_LENGTH) {
       debugf("Read %d/%d", bytesReadInPacket, MAX_PACKET_LENGTH);
       continue; //incomplete buffer, keep trying
     }

     debugf("read buffer from fd:%d in %d tries", fSockfd, attempts);

     totalBytesRead += bytesReadInPacket;
     bytesReadInPacket = 0;
     attempts = 0;
   }

   if (failure) {
     onRead(i, NULL, 0);
     this->onFailedConnection(i);
     continue;
   }

   if (totalBytesRead > 0) {
     onRead(i, buffer, totalBytesRead);
   }
  }
  return totalBytesRead;
}

int Socket::writeData(int fd, void* data, size_t size) {
  if (size <= 0 || NULL == data || !fConnected || !fReady || (!FD_ISSET (fd, &fMasterSet)))
    return -1;

  debugf("writing: %s, %u bytes", (char*) data, size);

  int bufferSize = min((int)size, MAX_PACKET_LENGTH);
  char buffer[bufferSize];
  memset(buffer, 0, bufferSize);
  memcpy(buffer, (char*)data, bufferSize);
  bool failure = false;

  int retval = write(fd, buffer, bufferSize);

  if (retval < 0) {
    if (errno == EPIPE) {
      debugf("broken pipe, client closed connection");
      failure = true;
    } else if (errno == EWOULDBLOCK || errno == EAGAIN) {
      // if (bytesWrittenInPacket > 0)
      //   continue; //incomplete buffer or frame, keep trying
      // else
      //   break; //client not available, skip current transfer
    } else {
      debugf("write(%d) failed with error:%s", fd, strerror(errno));
      failure = true;
    }
  }

  if (failure) {
    this->onFailedConnection(fd);
    return -1;
  }

  debugf("wrote %d bytes to fd:%d", retval, fd);
  return retval;
}
