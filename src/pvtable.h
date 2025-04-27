#ifndef PVTABLE_H
#define PVTABLE_H

#include "types.h"

struct S_BOARD;

struct S_PVENTRY{
    U64 posKey;
    int move;
};

struct S_PVTABLE{
    S_PVENTRY *pTable;
    int numEntries;

    S_PVTABLE() : pTable(NULL), numEntries(0) {}
};

int GetPvLine(const int depth, S_BOARD *pos);

void InitPvTable(S_PVTABLE *table);

void ClearPvTable(S_PVTABLE *table);

void StorePvMove(const S_BOARD *pos, const int move);

int ProbePvTable(const S_BOARD *pos);

#endif