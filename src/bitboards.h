#ifndef BITBOARDS_H
#define BITBOARDS_H

#include "types.h"

#include <iostream>

#define POP(b) PopBit(b)
#define CNT(b) CountBits(b)

extern const int BitTable[64];

int PopBit(U64 *bb);

int CountBits(U64 b);

void PrintBitBoard(U64 bb);
#endif