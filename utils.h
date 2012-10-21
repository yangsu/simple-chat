#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <cerrno>

using namespace std;

/**
 * Utility functions, definitions, and constants
 */

typedef void (*voidFunction)();

void print(string message) { cout << message << endl; }

void error(string message) {
  cerr << message << endl;
  if (errno != 0) {
    cerr << "\t error " << errno << ": " << strerror(errno) << endl;
  }
}

void noop() {};

#endif