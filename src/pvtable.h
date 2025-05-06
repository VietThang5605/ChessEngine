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
	int age;
};

struct S_HASHTABLE {
	S_HASHENTRY *pTable;
	int numEntries;
	int newWrite;
	int overWrite;
	int hit;
	int cut;
	int currentAge;

    S_HASHTABLE() : pTable(NULL), numEntries(0) {}
};

extern S_HASHTABLE HashTable[1];

int GetPvLine(const int depth, S_BOARD *pos, const S_HASHTABLE *table);

void InitHashTable(S_HASHTABLE *table, const int MB);

void ClearHashTable(S_HASHTABLE *table);

void StoreHashEntry(S_BOARD *pos, S_HASHTABLE *table, const int move, int score, const int flags, const int depth);

int ProbeHashEntry(S_BOARD *pos, S_HASHTABLE *table, int *move, int *score, int alpha, int beta, int depth);

int ProbePvMove(const S_BOARD *pos, const S_HASHTABLE *table);

#endif