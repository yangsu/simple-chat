#include <iostream>
#include <string>
#include <cstdlib>
#include <stdio.h>

#include "utils.h"
#include "chatserver.h"
#include "chatclient.h"
using namespace std;

// Move function declaration so cmds can be populated properly
void help();
Command cmds[] = {
  {"help", help, "help"},
  {"q", noop, "quit"}
};

void help () { printCommands(cmds); }

void processInput(string input) {
  if (execCommand(cmds, input)) {

  } else {

  }
}

int main(int argc, char *argv[]) {
  print("Simple Chat. Type \"help\" for a list of commands");

  readInput(processInput);

  return 0;
}