#include <iostream>
#include <string>
#include <cstdlib>
#include <stdio.h>

#include "utils.h"
#include "socket.h"
#include "chatserver.h"
#include "chatclient.h"
using namespace std;

ChatServer* server;
ChatClient* client;
void destroyServer(int count = 0, ...) {
  delete server;
  server = NULL;
}

void createServer(int count, ...) {
  va_list vl;
  va_start(vl, count);
  char** args = va_arg(vl, char**);
  va_end(vl);

  destroyServer();
  if (count) {
    int port = atoi(*args);
    server = new ChatServer(port);
  } else {
    server = new ChatServer();
  }
  server->listen();
}

void createClient(int count, ...) {
  if (count < 1) {
    printf("Must specify and ip\n");
  } else {
    va_list vl;
    va_start(vl, count);
    char** args = va_arg(vl, char**);
    va_end(vl);

    for (int i = 0; i < count; i++) {
      printf ("arg %s\n",args[i]);
    }

    if (client == NULL)
      client = new ChatClient();

    string ip = string(args[0], strlen(args[0]));
    printf("ip %s %i\n", ip.c_str(), (int)ip.length());

    if (count == 2) {
      int port = atoi(args[1]);
      client->connectToServer(ip, port);
    } else {
      client->connectToServer(ip, DEFAULT_PORT);
    }
  }
}

void getAvailableClients(int count = 0, ...) {
  if (client != NULL)
    client->getAvailableClients();
}

void destroyClient(int count = 0, ...) {
  delete client;
  client = NULL;
}

void quit(int count, ...) {
  destroyServer();
  destroyClient();
}

unsigned int size;
// Move function declaration so cmds can be populated properly
void help(int count, ...);
Command cmds[] = {
  {"s", createServer, "start server. can pass in an optional port number argument"},
  {"c", createClient, "start client. must specify and ip address. you can also pass in an optional port number argument"},
  {"cli", getAvailableClients, "get clients"},
  {"h", help, "help"},
  {"q", quit, "quit"}
};

void help (int count, ...) { printCommands(cmds, size); }

void processInput(string input) {
  if (execCommand(cmds, size, input)) {

  } else {
    if (client != NULL) {
      client->getAvailableClients();
    }
  }
}

int main(int argc, char *argv[]) {
  size = sizeof(cmds)/sizeof(Command);
  print("Simple Chat. Type \"help\" for a list of commands");

  readInput(processInput);

  return 0;
}