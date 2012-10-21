#include <cerrno>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include "utils.h"

using namespace std;

// Functions for commands
void noop() {};
void printCommands(Command cmds[]) {
  unsigned int end = sizeof(cmds)/sizeof(Command);
  printf("%u commands: \n", end);
  for (unsigned int i = 0; i < end; ++i) {
    printf("\t%s : %s\n", cmds[i].key.c_str(), cmds[i].description.c_str());
  }
}

bool execCommand(Command cmds[], string input) {
  for (unsigned int i = 0, end = sizeof(cmds)/sizeof(Command); i < end; ++i) {
    if (input.compare(cmds[i].key) == 0) {
      cmds[i].func();
      return true;
    }
  }
  return false;
}

bool readInput(void (*processInput)(string input)) {
  string input;

  while(true) {
    getline(cin, input);

    processInput(input);

    if (input.compare("q") == 0)
      break;
  }

  return true;
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    stringstream ss(s);
    string item;

    while(getline(ss, item, delim)) {
      elems.push_back(item);
    }

    return elems;
}

// Print functions
void print(string message) {
  cout << message << endl;
}

void debugf(string format, ...) {
  #ifdef DEBUG
    va_list args;
    va_start(args, format);
    vprintf(format.c_str(), args);
    va_end(args);
  #endif
}

void error(string message) {
  cerr << message << endl;
  if (errno != 0) {
    cerr << "\t error " << errno << ": " << strerror(errno) << endl;
  }
}