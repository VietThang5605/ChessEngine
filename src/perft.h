#ifndef PERFT_H
#define PERFT_H

#include "perft.h"
#include "board.h"
#include "movegen.h"
#include "makemove.h"
#include "init.h"
#include "validate.h"

extern long long leafNodes;

void Perft(int depth, S_BOARD *pos);

void PerftTest(int depth, S_BOARD *pos); //kindof a mirror of the perft function

#endif