#ifndef VALIDATE_H
#define VALIDATE_H

#include "types.h"
#include "board.h"

bool SqOnBoard(const int sq);

bool SideValid(const int side);

bool FileRankValid(const int fr);

bool PieceValidEmpty(const int pce);

bool PieceValid(const int pce);

bool MoveListOk(const S_MOVELIST *list,  const S_BOARD *pos);

void MirrorEvalTest(S_BOARD *pos);
#endif