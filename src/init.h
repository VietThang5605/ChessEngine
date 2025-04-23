#ifndef INIT_H
#define INIT_H

#include "types.h"

#include <cstdlib>
#include <ctime>

#define SQ64(sq120) (Sq120ToSq64[(sq120)])
#define SQ120(sq64) (Sq64ToSq120[(sq64)])
// #define CLRBIT(bb,sq)  ((bb) &= ClearMask[(sq)])
// #define SETBIT(bb,sq)  ((bb) |= SetMask[(sq)])
#define CLRBIT(bb, sq) clearBit(bb, sq)
#define SETBIT(bb, sq) setBit(bb, sq)

#define RAND_15 (rand() % 32768)

#define RAND_64 (   (U64)RAND_15 | \
                    (U64)RAND_15 << 15 | \
                    (U64)RAND_15 << 30 | \
                    (U64)RAND_15 << 45 | \
                    ((U64)RAND_15 & 0xf) << 60    )
//filling 64 bits woth random numbers
//generates a 15 bit random number
// first line: 0000 000000000000000 000000000000000 000000000000000 11111111111111
// seconde line // 0000 000000000000000 000000000000000 11111111111111  000000000000000
//and so on... shifting it 15 to the left
//leaves with 4 bits at the end and we take another random number and end it with hexa
// so we fill the 64 BIT at the end
//We're going to generate posKey by Zobrist hashing.

extern int Sq120ToSq64[BRD_SQ_NUM]; // will convert the index from the 120 squares board (with the grey things) to the "real" board of 64 cases
extern int Sq64ToSq120[64]; // literally the opposite of the previous

extern U64 SetMask[64]; //to set a bit on the bitboard (set it from 0 to 1)
extern U64 ClearMask[64];  //to clear a it from the bitboard (from 1 to 0)

extern U64 PieceKeys[13][120]; //#pieces index and each sqaure
extern U64 SideKey; //hashing in a random number if its white to move
// 0 0 0 0
extern U64 CastleKeys[16]; // 16 castle cases cuz bits are worth 1+2+4+8 = 16

void InitHashKeys();

void InitBitMasks();
void clearBit(U64 *bb, U64 sq);
void setBit(U64 *bb, U64 sq);

void InitSq120To64();

void InitSrandTime();

void AllInit();
#endif