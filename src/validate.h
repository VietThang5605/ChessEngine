#ifndef VALIDATE_H
#define VALIDATE_H

#include "types.h"
#include "io.h"

int SqOnBoard(const int sq);
int SideValid(const int side);
int PieceValidEmpty(const int pce);
int PieceValid(const int pce);
int SqIs120(const int sq);
int PceValidEmptyOffbrd(const int pce);
int MoveListOk(const S_MOVELIST *list, const S_BOARD *pos);
#endif