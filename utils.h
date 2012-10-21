#ifndef UTILS_H
#define UTILS_H

/**
 * Utility functions, definitions, and constants
 */

#include <string>
#include <cerrno>

#define DEBUG
#define MAX_PACKET_LENGTH 1024
#define DEFAULT_PORT 15555
#define MAX_CLIENTS 100

using namespace std;


typedef void (*voidFunction)();


void print(string message) {
  cout << message << endl;
}

void debugf(char* format, ...) {
  #ifdef DEBUG
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
  #endif
}

void error(string message) {
  cerr << message << endl;
  if (errno != 0) {
    cerr << "\t error " << errno << ": " << strerror(errno) << endl;
  }
}

void noop() {};

#endif