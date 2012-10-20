#include <iostream>
#include <string>
#include <cstdlib>
#include <stdio.h>

using namespace std;

void print(string message) { cout << message << endl; }

void noop() {};

typedef void (*cmdFunc)();
struct Command {
  string cmd;
  cmdFunc func;
  string funcName;
};

// Move function declaration so cmds can be populated properly
void help();
Command cmds[] = {
  {"help", help, "help"},
  {"q", noop, "quit"}
};

void help() {
  unsigned int end = sizeof(cmds)/sizeof(Command);
  cout << end << " commands:" << endl;
  for (unsigned int i = 0; i < end; ++i) {
    cout << "  " << cmds[i].cmd << " : " << cmds[i].funcName << endl;
  }
}

bool execCommand(string input) {
  for (unsigned int i = 0, end = sizeof(cmds)/sizeof(Command); i < end; ++i) {
    if (input.compare(cmds[i].cmd) == 0) {
      cmds[i].func();
      return true;
    }
  }
  return false;
}

int main() {
  print("Simple Chat. Type \"help\" for a list of commands");

  string input;
  while(true) {
    getline(cin,input);

    if (input.compare("q") == 0)
      break;

    if (execCommand(input)) {

    } else {

    }
  }
  return 0;
}