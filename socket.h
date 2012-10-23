#ifndef SOCKET_H
#define SOCKET_H

#include "utils.h"

#define MAX_PACKET_LENGTH 1024
#define HEADER_LENGTH 50
#define MESSAGE_LENGTH MAX_PACKET_LENGTH-HEADER_LENGTH
#define CONNECTION_TIMEOUT 10
#define POLLING_INTERVAL 50

enum MessageType {
  kClientList,
  kChatMessage,
  kUnkown
};
struct header {
  int targetId;
  MessageType type;
  size_t size;
  header() {
    this->targetId = -1;
    this->type = kUnkown;
    this->size = 0;
  }
  header(int id, MessageType t, size_t s) {
    this->targetId = id;
    this->type = t;
    this->size = s;
  }
};

class Socket {
  public:
    Socket();
    virtual ~Socket();

    bool isConnected() { return fReady && fConnected; }

    int writeData(int fd, header h, void* data);
    int readData(int fd, void (*onRead)(int cid, header h, const void* data));
    int readAll(void (*onRead)(int cid, header h, const void* data));

    /**
     * Close all open sockets
     */
    int closeAll();

  protected:
    /**
    * Create a socket and return its file descriptor. Returns -1 on failure
    */
    int create();

    /**
    * Close the socket specified by the socket file descriptor argument. Will
    * update fMaxfd and working set properly
    */
    void closeFd(int sockfd);

    /**
    * Called when a broken or terminated connection has been detected. Closes
    * the socket file descriptor and removes it from the master set by default.
    * Override to handle broken connections differently
    */
    virtual void onFailedConnection(int sockfd);

    /**
     * Set the socket specified by the socket file descriptor as nonblocking
     */
    void setNonBlocking(int sockfd);

    /**
    * Add the socket specified by the socket file descriptor to the master
    * file descriptor set, which is used to in the select() to detect new data
    * or connections
    */
    void addToMasterSet(int sockfd);

    bool    fConnected;
    bool    fReady;
    int     fMaxfd;
    int     fPort;
    int     fSockfd;

    /**
    * fMasterSet contains all the file descriptors to be used for read/write.
    * For clients, this only contains the client socket. For servers, this
    * contains all the file descriptors associated with established connections
    * to clients
    */
    fd_set  fMasterSet;
};
#endif