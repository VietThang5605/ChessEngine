#include "types.h"
#include "board.h"
#include "data.h"

const int KnightDir[8] = { -8, -19,	-21, -12, 8, 19, 21, 12 }; //looking at the board representation in the excel file, knights are positionned like this from the current square
const int RookDir[4] = { -1, -10,	1, 10 }; // for rooks,, its simple, its just +9
const int BishopDir[4] = { -9, -11, 11, 9 }; // for bishops
const int KingDir[8] = { -1, -10,	1, 10, -9, -11, 11, 9 }; //king direction can move only one square to every direction

bool SqAttacked(const int sq, const int side, const S_BOARD *pos);