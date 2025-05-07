#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"
#include "board.h"
#include "pvtable.h"
#include "tinycthread.h"

struct S_SEARCHINFO {
    int startTime;
    int stopTime;

    int depth;
    bool depthSet;
    
    bool timeSet;
    int movesToGo;
    int infinite;

    long long nodes;

    bool quit;
    bool stopped;

    float fh;
    float fhf;
    int nullCut;

    int threadNumber;
};

struct S_SEARCH_THREAD_DATA {
    S_SEARCHINFO *info;
    S_BOARD *originalPosition;
    S_HASHTABLE *ttable;
};

struct S_SEARCH_WORKER_DATA {
    S_BOARD *pos;
    S_SEARCHINFO *info;
    S_HASHTABLE *ttable;

    int threadNumber;
    int depth;
    int bestMove;
};

extern int rootDepth;

extern thrd_t workerThreads[MAXTHREAD];

static void CheckUp(S_SEARCHINFO *info);

static void PickNextMove(int moveNum, S_MOVELIST *list);

bool IsRepetition(const S_BOARD *pos);

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table);

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info);

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table, int DoNull);

int SearchPosition_Thread(void *data);

void IterativeDeepening(S_SEARCH_WORKER_DATA *workerData);

int StartWorkerThread(void *data);

void SetupWorker(int threadNum, thrd_t *workerTh, S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table);

void CreateSearchWorkers(S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table);

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table);

#endif