#ifndef POLYBOOK_H
#define POLYBOOK_H

#include "types.h"
#include "board.h"

extern const int PolyKindOfPiece[13];

bool HasPawnForCapture(const S_BOARD *board);

U64 PolyKeyFromBoard(const S_BOARD *board);

#endif