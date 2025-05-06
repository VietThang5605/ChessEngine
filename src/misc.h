#ifndef MISC_H
#define MISC_H

#include <chrono>

typedef std::chrono::milliseconds::rep TimePoint; // A value in milliseconds

#ifdef WIN32
#include "windows.h"
#include <io.h>
#else
#include "sys/time.h"
#endif

int GetTimeMs();

#endif