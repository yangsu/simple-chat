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
  if (server) {
    delete server;
    server = NULL;
  }
}

void createServer(int count, ...) {
  va_list vl;
  va_start(vl, count);
  char** val=va_arg(vl,char**);
  va_end(vl);

  for (int i = 0; i < count; i++) {
    printf ("arg %s\n",val[i]);
  }

  destroyServer();
  server = new ChatServer();
}

unsigned int size;
// Move function declaration so cmds can be populated properly
void help(int count, ...);
Command cmds[] = {
  {"server", createServer, "start server. can pass in an optional port number argument"},
  {"help", help, "help"},
  {"q", noop, "quit"}
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