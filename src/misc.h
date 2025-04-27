#ifndef MISC_H
#define MISC_H

#include "types.h"

#include <chrono>

typedef std::chrono::milliseconds::rep TimePoint; // A value in milliseconds

// #ifdef WIN32
// #include "windows.h"
// #else
// #include "sys/time.h"
// #include "sys/select.h"
// #include "unistd.h"
// #include "string.h"
// #endif

int GetTimeMs();


#endif