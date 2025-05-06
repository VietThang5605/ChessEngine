#include "misc.h"

#include <stdio.h>

int GetTimeMs() {
// #ifdef WIN32
//     return GetTickCount();
// #else
//     struct timeval t;
//     gettimeofday(&t, NULL);
//     return t.tv_sec*1000 + t.tv_usec/1000;
// #endif
    return std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::steady_clock::now().time_since_epoch()).count();
}