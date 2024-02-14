// Debug.h
#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG 0

#if DEBUG
#include <iostream>
#define DEBUG_COUT std::cout
#else
#include <ostream>
#include <iostream>
#include <sstream>
static std::ostringstream null_stream;
#define DEBUG_COUT null_stream
#endif

#endif // DEBUG_H