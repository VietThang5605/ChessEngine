#ifndef VALIDATE_H
#define VALIDATE_H

#include "types.h"
#include "init.h"

bool SqOnBoard(const int sq);

bool SideValid(const int side);

bool FileRankValid(const int fr);

bool PieceValidEmpty(const int pce);

bool PieceValid(const int pce);
#endif