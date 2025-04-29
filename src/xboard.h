#ifndef XBOARD_H
#define XBOARD_H

#include "board.h"
#include "search.h"

bool ThreeFoldRep(const S_BOARD *pos);

bool DrawMaterial(const S_BOARD *pos);

bool checkResult(S_BOARD *pos);

void PrintOptions();

void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info);

void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info);

#endif