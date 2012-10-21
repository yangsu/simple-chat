#ifndef UTILS_H
#define UTILS_H

/**
 * Utility functions, definitions, and constants
 */

#include <string>
#include <vector>

#define DEBUG
#define MAX_PACKET_LENGTH 1024
#define DEFAULT_PORT 15555
#define MAX_CLIENTS 100

using namespace std;

// Functions for commands
typedef void (*voidFunction)();
void noop();

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