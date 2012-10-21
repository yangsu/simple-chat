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

  this->setNonBlocking(sockfd);

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

int Socket::closeAllSockets() {
  if (!fConnected || !fReady)
    return -1;
  for (int i = 0; i <= fMaxfd; ++i) {
    if (FD_ISSET(i, &fMasterSet))
      this->closeSocket(i);
  }
  fConnected = false;
  return 0;
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

  // char packet[MAX_PACKET_LENGTH];
  for (int i = 0; i <= fMaxfd; ++i) {
   if (!FD_ISSET (i, &fMasterSet))
     continue;

   memset(packet, 0, MAX_PACKET_LENGTH);

   int attempts = 0;
   bool failure = false;

   int bytesReadInPacket = 0;

   while (fConnected && !failure) {
     int retval = read(i, packet + bytesReadInPacket,
               MAX_PACKET_LENGTH - bytesReadInPacket);

     ++attempts;

     if (retval < 0) {
       if (errno == EWOULDBLOCK || errno == EAGAIN) {
         if (bytesReadInPacket > 0)
           continue; //incomplete packet or frame, keep tring
         else
           break; //nothing to read
       }
       debugf("Read() failed with error: %s\n", strerror(errno));
       failure = true;
       break;
     }

     if (retval == 0) {
       debugf("Peer closed connection or connection failed\n");
       failure = true;
       break;
     }

     bytesReadInPacket += retval;
     if (bytesReadInPacket < MAX_PACKET_LENGTH) {
       debugf("Read %d/%d\n", bytesReadInPacket, MAX_PACKET_LENGTH);
       continue; //incomplete packet, keep trying
     }

     debugf("read packet from fd:%d in %d tries\n", fSockfd, attempts);

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
     onRead(i, packet, totalBytesRead);
   }
  }
  return totalBytesRead;
}

int Socket::writePacket(void* data, size_t size) {
  if (size <= 0 || NULL == data || !fConnected || !fReady)
    return -1;

  int totalBytesWritten = 0;

  char packet[MAX_PACKET_LENGTH];
  for (int i = 0; i <= fMaxfd; ++i) {
    if (!FD_ISSET (i, &fMasterSet))
      continue;

    int bytesWrittenInPacket = 0;
    int attempts = 0;
    bool failure = false;
    while (bytesWrittenInPacket < size && fConnected && !failure) {
      memset(packet, 0, MAX_PACKET_LENGTH);
      memcpy(packet + HEADER_SIZE, (char*)data, size);

      int retval = write(i, packet + bytesWrittenInPacket,
        MAX_PACKET_LENGTH - bytesWrittenInPacket);
      attempts++;

      if (retval < 0) {
        if (errno == EPIPE) {
          debugf("broken pipe, client closed connection");
          failure = true;
          break;
        } else if (errno == EWOULDBLOCK || errno == EAGAIN) {
          if (bytesWrittenInPacket > 0)
            continue; //incomplete packet or frame, keep trying
          else
            break; //client not available, skip current transfer
        } else {
          debugf("write(%d) failed with error:%s\n", i, strerror(errno));

          failure = true;
          break;
        }
      }

      bytesWrittenInPacket += retval;
      if (bytesWrittenInPacket < MAX_PACKET_LENGTH)
        continue; //incomplete packet, keep trying

      debugf("wrote packet to fd:%d in %d tries\n", i, attempts);

      totalBytesWritten = bytesWrittenInPacket;
      bytesWrittenInPacket = 0;
      attempts = 0;
    }

    if (failure)
      this->onFailedConnection(i);
  }
  return totalBytesWritten;
}
