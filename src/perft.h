#ifndef PERFT_H
#define PERFT_H

#include "types.h"
#include "board.h"

extern long long leafNodes;

void Perft(int depth, S_BOARD *pos);

void PerftTest(int depth, S_BOARD *pos); //kindof a mirror of the perft function

#endif