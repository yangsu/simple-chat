#include <cerrno>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <vector>
#include "utils.h"
#include <stdarg.h>
#include <string.h>
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

int randomPort() {
  return rand() % 35535 + 20000;
}

void printCommands(Command cmds[], unsigned int size) {
  printf("%u commands: \n", size);
  for (unsigned int i = 0; i < size; ++i) {
    printf("\t%s : %s\n", cmds[i].key.c_str(), cmds[i].description.c_str());
  }
}

string ipstring(string ip, int port) {
  char ipbuffer[50];
  int length = sprintf(ipbuffer, "%s:%d", ip.c_str(), port);
  return string(ipbuffer, length);
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
  if (input.length() > 0) {
    vector<string> args = split(input, ' ');
    string command = (args.size() == 0) ? input : args[0];
    for (unsigned int i = 0; i < size; ++i) {
      if (command.compare(cmds[i].key) == 0) {
        args.erase(args.begin());
        if (args.size() > 0) {
          cmds[i].func(args.size(), (char**)&(args[0]));
        } else {
          cmds[i].func(args.size());
        }

        return true;
      }
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
    format.insert(0, "\t");
    format += "\n";
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