#ifndef MAKEMOVE_H
#define MAKEMOVE_H

#include "types.h"
#include "board.h"
#include "init.h"

#define HASH_PIECE(piece, sq) (pos->posKey ^= (PieceKeys[(piece)][(sq)]))
#define HASH_CASTLE (pos->posKey ^= (CastleKeys[(pos->castlePerm)]))
#define HASH_SIDE (pos->posKey ^= (SideKey))
#define HASH_EP (pos->posKey ^= (PieceKeys[EMPTY][(pos->enPas)]))

const int CastlePerm[120] = { // castle permsissions array
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 7, 15, 15, 15, 3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15}; // its a mirror of our 120 squares board, all set to 15 except A1 E1 H1 8 E8 h8
// every time we move a piece, we take the catle persmission and we dop a bitwise AND operation with this array and the square from array

static void ClearPiece(const int sq, S_BOARD *pos);

static void AddPiece(const int sq, S_BOARD *pos, const int piece); // basically the opposite of clearpiece

static void MovePiece(const int from, const int to, S_BOARD *pos);

bool MakeMove(S_BOARD *pos, int move);

void TakeMove(S_BOARD *pos); // similar to makemove, not a lot to explain

#endif