#ifndef PVTABLE_H
#define PVTABLE_H

#include "types.h"

struct S_BOARD;

struct S_HASHENTRY {
	U64 posKey;
	int move;
	int score;
	int depth;
	int flags;
};

struct S_HASHTABLE{
	S_HASHENTRY *pTable;
	int numEntries;
	int newWrite;
	int overWrite;
	int hit;
	int cut;

    S_HASHTABLE() : pTable(NULL), numEntries(0) {}
};

int GetPvLine(const int depth, S_BOARD *pos);

void InitHashTable(S_HASHTABLE *table, const int MB);

void ClearHashTable(S_HASHTABLE *table);

void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth);

int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth);

int ProbePvMove(const S_BOARD *pos);

#endif