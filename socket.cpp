#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "utils.h"
#include "socket.h"
#include <cstring>

Socket::Socket() {
  fMaxfd = 0;
  FD_ZERO(&fMasterSet);
  fConnected = false;
  fReady = false;
  fSockfd = this->create();

  fIP = this->getLocalAddr();
  fPort = randomPort();

  debugf("%s:%d", fIP.c_str(), fPort);
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
  char* buffer = (char*)malloc(size*sizeof(char));
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
      bytesRead += retval;
      debugf("read %u/%d bytes from fd:%d", bytesRead, size, fd);
    } else {
      debugf("Peer closed connection or connection failed");
      return -1;
    }
  }
  if (size > 0) {
    memcpy(dest, buffer, size);
    ((char*)dest)[size] = '\0';
    // debugf("dest %s", dest);
  }
  free(buffer);
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
  memset(headerBuffer, 0, HEADER_LENGTH);

  int bytesRead = readFromSocket(fd, headerBuffer, HEADER_LENGTH);
  if (bytesRead == HEADER_LENGTH) {
    memcpy(&(h->targetId), headerBuffer, sizeof(int));
    memcpy(&(h->sourceId), headerBuffer + sizeof(int), sizeof(int));
    memcpy(&(h->type), headerBuffer + 2*sizeof(int), sizeof(MessageType));
    memcpy(&(h->size), headerBuffer + 2*sizeof(int) + sizeof(MessageType), sizeof(int));
    debugf("read header(%d, %d, %d, %d)", h->targetId, h->sourceId, h->type, h->size);
    return bytesRead;
  } else if (bytesRead >= 0) {
    return bytesRead;
  } else {
    return -1;
  }
}

int Socket::readData(void (*onRead)(int, header, const void*)) {
  return this->readDataFromFd(fSockfd, onRead);
}

int Socket::readDataFromFd(int fd, void (*onRead)(int, header, const void*)) {
  if (!fConnected || !fReady || NULL == onRead || !FD_ISSET(fd, &fMasterSet))
    return -1;

  header h;
  if (readHeader(fd, &h) < 0) {
    this->onFailedConnection(fd);
    return -1;
  }

  int bufferSize = min((int)h.size, MESSAGE_LENGTH);

  char *buffer = (char*)malloc(bufferSize*sizeof(char));
  memset(buffer, 0, bufferSize);

  int bytesRead = readFromSocket(fd, buffer, bufferSize);
  if (bytesRead == bufferSize) {
    onRead(fd, h, buffer);
  } else {
    this->onFailedConnection(fd);
  }

  free(buffer);
  return bytesRead;
}

int Socket::writeData(header h, void* data) {
  return this->writeDataToFd(fSockfd, h, data);
}

int Socket::writeDataToFd(int fd, header h, void* data) {
  if (h.size <= 0 || NULL == data || !fConnected || !fReady || (!FD_ISSET (fd, &fMasterSet)))
    return -1;

  debugf("writing: %s, %u bytes", (char*) data, h.size);

  int messageSize = min((int)h.size, MESSAGE_LENGTH);
  int bufferSize = messageSize + HEADER_LENGTH;

  char *buffer = (char*)malloc(bufferSize*sizeof(char));
  memset(buffer, 0, bufferSize);

  memcpy(buffer, &(h.targetId), sizeof(int));
  memcpy(buffer + sizeof(int), &(h.sourceId), sizeof(int));
  memcpy(buffer + 2*sizeof(int), &(h.type), sizeof(MessageType));
  memcpy(buffer + 2*sizeof(int) + sizeof(MessageType), &(h.size), sizeof(int));

  debugf("wrote header(%d, %d, %d, %d)", h.targetId, h.sourceId, h.type, h.size);

  memcpy(buffer + HEADER_LENGTH, (char*)data, messageSize);
  buffer[HEADER_LENGTH + messageSize] = '\0';

  int bytesWritten = writeToSocket(fd, buffer, bufferSize);

  free(buffer);

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

string Socket::getLocalAddr() {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);
  if (getsockname(this->fSockfd, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0) {
    error("Could not get address of peer");
  }
  return inet_ntoa(addr.sin_addr);
}

int Socket::getLocalPort() {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);
  if (getsockname(this->fSockfd, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
    error("Could not get port of peer");
  }
  return (int)ntohs(addr.sin_port);
}

string Socket::getRemoteAddr() {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);
  if (getpeername(this->fSockfd, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0) {
    error("Could not get address of peer");
  }
  return inet_ntoa(addr.sin_addr);
}

int Socket::getRemotePort() {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);
  if (getpeername(this->fSockfd, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
    error("Could not get port of peer");
  }
  return (int)ntohs(addr.sin_port);
}