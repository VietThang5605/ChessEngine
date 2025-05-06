#ifndef UCI_H
#define UCI_H

#include "types.h" //must before tinycthread to using it's FALSE
#include "board.h"
#include "search.h"
#include "tinycthread.h"

extern thrd_t mainSearchThread;

thrd_t LaunchSearchThread(S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table);

void JoinSearchThread(S_SEARCHINFO *info);

void ParseGo(char* line, S_SEARCHINFO *info, S_BOARD *pos, S_HASHTABLE *table);

void ParsePosition(char* lineIn, S_BOARD *pos);

void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info);

#endif