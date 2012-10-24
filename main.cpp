#include <iostream>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <pthread.h>
#include <vector>
#include <map>

#include "utils.h"
#include "socket.h"
#include "chatserver.h"
#include "chatclient.h"
using namespace std;

vector<pthread_t*> threads;
bool terminateServer = false;
bool terminateClient = false;

/**
 * Server
 */

void* server(void* port) {
  int pn = (long)port;

  debugf("creating server with port %d", port);
  ChatServer server(pn);
  while (!terminateServer) {
    server.listen();
  }
  debugf("Server Thread Terminated");
}

void createServer(int count, ...) {
  va_list vl;
  va_start(vl, count);
  char** args = va_arg(vl, char**);
  va_end(vl);

  int port = (count > 0) ? atoi(*args) : DEFAULT_PORT;

  pthread_t* p = new pthread_t;
  threads.push_back(p);
  pthread_create(p, NULL, server, (void*)port);
}

/**
 * Client
 */
ChatClient* client;
void* clientf(void* data) {
  IPAddr* i = (IPAddr*)data;

  if (client == NULL)
    client = new ChatClient();

  client->connectToServer(i->ip, i->port);
  delete i;

  while(!terminateClient) {
    client->read();
  }

  delete client;

  debugf("terminating client");
}

void createClient(int count, ...) {
  if (count < 1) {
    printf("Must specify an ip\n");
  } else {
    va_list vl;
    va_start(vl, count);
    char** args = va_arg(vl, char**);
    va_end(vl);

    string ip = string(args[0], strlen(args[0]));
    int port = (count == 2) ? atoi(args[1]) : DEFAULT_PORT;
    IPAddr* i = new IPAddr(ip, port);
    pthread_t* p = new pthread_t;
    threads.push_back(p);
    pthread_create(p, NULL, clientf, (void*)i);
  }
}

void getAvailableClients(int count = 0, ...) {
  if (client != NULL) {
    client->getAvailableClients();
  } else {
    printf("Must be create a client to issue this command\n");
  }
}

void connectToClient(int count = 0, ...) {
  if (count < 1) {
    printf("Must specify an user id\n");
  } else if (client == NULL) {
    printf("Must be create a client to issue this command\n");
  } else {
    va_list vl;
    va_start(vl, count);
    char** args = va_arg(vl, char**);
    va_end(vl);

    int id = atoi(args[1]);
    debugf("connecting to User %d", id);
    client->connectToClient(id);
  }
}

void destroyClient(int count = 0, ...) {
  delete client;
  client = NULL;
}

void quit(int count, ...) {
  terminateServer = true;
  terminateClient = true;
  for (int i = 0; i < threads.size(); ++i) {
    debugf("waiting for thread %d", i);
    pthread_join(*(threads[i]), NULL);
    delete threads[i];
  }
}

unsigned int size;
// Move function declaration so cmds can be populated properly
void help(int count, ...);
Command cmds[] = {
  {"s", createServer, "start server. can pass in an optional port number argument"},
  {"c", createClient, "start client. must specify and ip address. you can also pass in an optional port number argument"},
  {"cli", getAvailableClients, "get clients"},
  {"connect", connectToClient, "connect to another client"},
  {"h", help, "help"},
  {"q", quit, "quit"}
};

void help (int count, ...) { printCommands(cmds, size); }

void processInput(string input) {
  if (execCommand(cmds, size, input)) {

  } else {
  }
}

int main(int argc, char *argv[]) {
  size = sizeof(cmds)/sizeof(Command);
  print("Simple Chat. Type \"help\" for a list of commands");

  readInput(processInput);

  return 0;
}