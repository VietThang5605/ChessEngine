#ifndef IO_H
#define IO_H

#include "types.h"
#include "board.h"

struct S_MOVE
{
    int move;
    int score;
};

struct S_MOVELIST
{
    S_MOVE moves[MAXPOSITIONMOVES];
    int count;
};

int ParseMove(char *ptrChar, S_BOARD *pos);

#endif