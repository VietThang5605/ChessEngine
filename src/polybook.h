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

static unsigned short endian_swap_u16(unsigned short x);

static unsigned int endian_swap_u32(unsigned int x);

static U64 endian_swap_u64(U64 x);

static int ConvertPolyMoveToInternalMove(unsigned short polyMove, S_BOARD *board);

int GetBookMove(S_BOARD *board);

#endif