#ifndef UTILS_H
#define UTILS_H

/**
 * Utility functions, definitions, and constants
 */

#include <string>
#include <vector>

// #define DEBUG
#define DEFAULT_PORT 15555
#define MAX_CLIENTS 100

using namespace std;

typedef void* (*voidStarFunction)(void*);

// Functions for commands
typedef void (*voidFunction)(int count, ...);
void noop(int count, ...);
void printArgs(int count, ...);

struct IPAddr {
  IPAddr(string i, int p) {
    this->ip = i;
    this->port = p;
  }
  string ip;
  int port;
};

struct Command {
  string key;
  voidFunction func;
  string description;
};

void printCommands(Command cmds[], unsigned int size);
bool execCommand(Command cmds[], unsigned int size, string input);
bool readInput(void (*processInput)(string input));
vector<string> split(const string &s, char delim);

// Print functions
void print(string message);
void debugf(string format, ...);
void error(string message);

#endif