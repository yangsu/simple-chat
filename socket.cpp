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

int readFromSocket(int fd, void* dest, int size) {
  char buffer[size];
  memset(buffer, 0, size);

  int retval = 0;
  int bytesRead = 0;
  while (bytesRead < size) {
    retval = read(fd, buffer + bytesRead, size - bytesRead);
    if (retval < 0) {
      if (errno == EWOULDBLOCK || errno == EAGAIN) {
        if (bytesRead > 0)
          return bytesRead; //incomplete buffer or frame, keep tring
        else
          return 0; //nothing to read
      } else {
        debugf("Read() failed with error: %s", strerror(errno));
        return -1;
      }
    } else if (retval > 0) {
      debugf("read %u bytes from fd:%d", retval, fd);
      bytesRead += retval;
    } else {
      debugf("Peer closed connection or connection failed");
      return -1;
    }
  }
  memcpy(dest, buffer, size);
  return size;
}

int writeToSocket(int fd, char* buffer, int size) {
  int retval = 0, bytesWritten = 0;
  while (bytesWritten < size) {
    retval = write(fd, buffer + bytesWritten, size - bytesWritten);
    if (retval < 0) {
      if (errno == EPIPE) {
        debugf("broken pipe, client closed connection");
        return -1;
      } else if (errno == EWOULDBLOCK || errno == EAGAIN) {
        if (bytesWritten > 0)
          return bytesWritten; //incomplete buffer or frame, keep trying
        else
          return 0; //client not available, skip current transfer
        debugf("write(%d) failed with error:%s", fd, strerror(errno));
        return -1;
      } else {
        debugf("write(%d) failed with error:%s", fd, strerror(errno));
        return -1;
      }
    }
    bytesWritten += retval;
  }
  return bytesWritten;
}

int readHeader(int fd, header* h) {
  char headerBuffer[HEADER_LENGTH];
  memset(&headerBuffer, 0, HEADER_LENGTH);

  int bytesRead = readFromSocket(fd, headerBuffer, HEADER_LENGTH);
  if (bytesRead == HEADER_LENGTH) {
    memcpy(&h->targetId, headerBuffer, sizeof(int));
    memcpy(&h->type, headerBuffer + sizeof(int), sizeof(MessageType));
    memcpy(&h->size, headerBuffer + sizeof(int) + sizeof(MessageType), sizeof(size_t));
    return bytesRead;
  } else if (bytesRead >= 0) {
    return bytesRead;
  } else {
    return -1;
  }
}

int Socket::readData(int fd, void (*onRead)(int, header, const void*)) {
  if (!fConnected || !fReady || NULL == onRead || !FD_ISSET(fd, &fMasterSet))
    return -1;

  header h;
  if (readHeader(fd, &h) < 0) {
    this->onFailedConnection(fd);
    return -1;
  }

  int bufferSize = min((int)h.size, MESSAGE_LENGTH);
  char buffer[bufferSize];
  memset(buffer, 0, bufferSize);

  int bytesRead = readFromSocket(fd, &buffer, bufferSize);
  if (bytesRead == bufferSize) {
    onRead(fd, h, buffer);
    return bytesRead;
  } else {
    this->onFailedConnection(fd);
    return -1;
  }
}

int Socket::readAll(void (*onRead)(int, header, const void*)) {
  int total = 0;
  for (int i = 0; i <= fMaxfd; ++i) {
    if (FD_ISSET(i, &fMasterSet)) {
      total += this->readData(i, onRead);
    }
  }
  return total;
}

int Socket::writeData(int fd, header h, void* data) {
  if (h.size <= 0 || NULL == data || !fConnected || !fReady || (!FD_ISSET (fd, &fMasterSet)))
    return -1;

  debugf("writing: %s, %u bytes", (char*) data, h.size);

  int messageSize = min((int)h.size, MESSAGE_LENGTH);
  int bufferSize = messageSize + HEADER_LENGTH;
  char buffer[bufferSize];
  memset(buffer, 0, bufferSize);

  memcpy(&buffer, &(h.targetId), sizeof(int));
  memcpy(&buffer + sizeof(int), &(h.type), sizeof(MessageType));
  memcpy(&buffer + sizeof(int) + sizeof(MessageType), &(h.size), sizeof(size_t));

  memcpy(buffer + HEADER_LENGTH, (char*)data, messageSize);

  int bytesWritten = writeToSocket(fd, buffer, bufferSize);

  if (bytesWritten == bufferSize) {
    debugf("wrote %d bytes to fd:%d", bytesWritten, fd);
    return bytesWritten;
  } else if (bytesWritten >= 0) {
    return bytesWritten;
  } else {
    this->onFailedConnection(fd);
    return -1;
  }
}
