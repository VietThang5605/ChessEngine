#ifndef BITBOARDS_H
#define BITBOARDS_H

#include "types.h"

#include <iostream>

#define POP(b) PopBit(b)
#define CNT(b) CountBits(b)

extern const int BitTable[64];

extern const int RookShifts[64];
extern const int BishopShifts[64];

extern const U64 RookMagics[64];
extern const U64 BishopMagics[64];

extern U64 RookMask[64];
extern U64 BishopMask[64];

extern U64 *RookAttacks[64];
extern U64 *BishopAttacks[64];

int PopBit(U64 *bb);

int CountBits(U64 b);

void PrintBitBoard(U64 bb);

U64 GetSliderAttacks(int square, U64 blockers, bool ortho);

static U64 GetRookAttacks(int square, U64 blockers);

static U64 GetBishopAttacks(int square, U64 blockers);

static void CreateMovementMask(int squareIndex, bool rook);

static U64 LegalMoveBitboardFromBlockers(int square, U64 blockerBitboard, bool rook);

static void CreateTable(int square, bool rook, U64 magic, int leftShift);

void InitMagic();

void FreeMagic();

#endif