#ifndef IO_H
#define IO_H

#include "types.h"
#include "init.h"
#include "data.h"
#include "board.h"
#include "validate.h"
#include "movegen.h"

char *PrintSquare(const int sq); // simple function, to describe squares from a bit array to something more "human" such as B2B3 etc...

char *PrintMove(const int move);

int ParseMove(char *ptrChar, S_BOARD *pos);

void PrintMoveList(const S_MOVELIST *list);
#endif