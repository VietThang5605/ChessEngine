#ifndef UCI_H
#define UCI_H

#include "types.h"
#include "board.h"
#include "search.h"

#include <cstring>

#define INPUTBUFFER 400 * 6

void ParseGo(char* line, S_SEARCHINFO *info, S_BOARD *pos);
void ParsePosition(char* lineIn, S_BOARD *pos);

void Uci_Loop();

#endif