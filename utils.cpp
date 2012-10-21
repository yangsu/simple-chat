#include <cerrno>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include "utils.h"

using namespace std;

// vector<string> noargs(0);
// Functions for commands
void noop(int count, ...) {};

void printArgs(int count, ...) {
  va_list vl;
  va_start(vl, count);
  char** args = va_arg(vl, char**);
  va_end(vl);

  for (int i = 0; i < count; i++) {
    printf ("arg %s\n",args[i]);
  }
};

void printCommands(Command cmds[], unsigned int size) {
  printf("%u commands: \n", size);
  for (unsigned int i = 0; i < size; ++i) {
    printf("\t%s : %s\n", cmds[i].key.c_str(), cmds[i].description.c_str());
  }
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

bool execCommand(Command cmds[], unsigned int size, string input) {
  for (unsigned int i = 0; i < size; ++i) {
    vector<string> args = split(input, ' ');
    if (args[0].compare(cmds[i].key) == 0) {
      args.erase(args.begin());
      cmds[i].func(args.size(), (char**)&(args[0]));

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

    if (input.compare("q") == 0) {
      return true;
    }
  }
  return true;
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