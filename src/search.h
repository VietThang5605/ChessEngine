#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"
#include "board.h"

struct S_SEARCHINFO {
    int startTime;
    int stopTime;

    int depth;
    int depthSet;
    
    int timeSet;
    int movesToGo;
    int infinite;

    long long nodes;

    bool quit;
    bool stopped;

    float fh;
    float fhf;
};

static void CheckUp(S_SEARCHINFO *info);

static void PickNextMove(int moveNum, S_MOVELIST *list);

bool IsRepetition(const S_BOARD *pos);

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info);

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info);

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull);

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);

#endif