#ifndef HASHKEYS_H
#define HASHKEYS_H

#include "types.h"

struct S_BOARD;

U64 GeneratePosKey(const S_BOARD *pos);
#endif