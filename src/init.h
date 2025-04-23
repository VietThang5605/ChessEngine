#ifndef INIT_H
#define INIT_H

#include "types.h"

#include <stdlib.h>

#define SQ64(sq120) (Sq120ToSq64[(sq120)])
#define SQ120(sq64) (Sq64ToSq120[(sq64)])
// #define CLRBIT(bb,sq)  ((bb) &= ClearMask[(sq)])
// #define SETBIT(bb,sq)  ((bb) |= SetMask[(sq)])
#define CLRBIT(bb, sq) clearBit(bb, sq)
#define SETBIT(bb, sq) setBit(bb, sq)

extern int Sq120ToSq64[BRD_SQ_NUM]; // will convert the index from the 120 squares board (with the grey things) to the "real" board of 64 cases
extern int Sq64ToSq120[64]; // literally the opposite of the previous

extern U64 SetMask[64]; //to set a bit on the bitboard (set it from 0 to 1)
extern U64 ClearMask[64];  //to clear a it from the bitboard (from 1 to 0)

void InitBitMasks();
void clearBit(U64 *bb, U64 sq);
void setBit(U64 *bb, U64 sq);

void InitSq120To64();

void AllInit();
#endif