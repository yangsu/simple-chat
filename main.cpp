#include <iostream>
#include <string>
#include <cstdlib>
#include <stdio.h>

#include "utils.h"
#include "socket.h"
#include "chatserver.h"
// #include "chatclient.h"
using namespace std;

ChatServer* server;
void destroyServer(int count = 0, ...) {
  if (server != NULL) {
    delete server;
    server = NULL;
  }
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
}

void createClient(int count, ...) {
  va_list vl;
  va_start(vl, count);
  char** args = va_arg(vl, char**);
  va_end(vl);

  for (int i = 0; i < count; i++) {
    printf ("arg %s\n",args[i]);
  }

  destroyServer();
  server = new ChatServer();
}

void quit(int count, ...) {
  destroyServer();
}

unsigned int size;
// Move function declaration so cmds can be populated properly
void help(int count, ...);
Command cmds[] = {
  {"server", createServer, "start server. can pass in an optional port number argument"},
  {"help", help, "help"},
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