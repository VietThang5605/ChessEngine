#ifndef UCI_H
#define UCI_H

#include <cstdio>
#include <cstring>

#include "types.h"
#include "board.h"
#include "init.h"

void ParseGo(char *line, S_SEARCHINFO *info, S_BOARD *pos);
void ParsePosition(char *lineIn, S_BOARD *pos);
void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info);
#endif