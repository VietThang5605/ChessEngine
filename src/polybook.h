#ifndef POLYBOOK_H
#define POLYBOOK_H

#include "types.h"
#include "board.h"

struct S_POLY_BOOK_ENTRY{
	U64 key;
	unsigned short move;
	unsigned short weight;
	unsigned int learn;
};

extern long long NumEntries;

extern S_POLY_BOOK_ENTRY *entries;

extern const int PolyKindOfPiece[13];

void InitPolyBook();

void CleanPolyBook();

bool HasPawnForCapture(const S_BOARD *board);

U64 PolyKeyFromBoard(const S_BOARD *board);

#endif