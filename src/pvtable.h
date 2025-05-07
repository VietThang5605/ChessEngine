#ifndef PVTABLE_H
#define PVTABLE_H

#include "types.h"

#include "io.h" // for debug
#include "makemove.h"
#include "movegen.h"

#define EXTRACT_SCORE(x) ((x & 0xFFFF) - INF_BOUND)
#define EXTRACT_DEPTH(x) ((x >> 16) & 0x3F)
#define EXTRACT_FLAGS(x) ((x >> 23) & 0x3)
#define EXTRACT_MOVE(x) ((int)(x >> 25))

#define FOLD_DATA(sc, de, fl, mv) ((sc + INF_BOUND) | (de << 16) | (fl << 23)  | ((U64)mv << 25))

struct S_BOARD;

struct S_HASHENTRY {
	U64 posKey;
	int move;
	int score;
	int depth;
	int flags;
	int age;
	U64 smp_key;
	U64 smp_data;
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

void DataCheck(const int move);

void TempHashTest(char *fen);

void VerifyEntrySMP(S_HASHENTRY *entry);

int GetPvLine(const int depth, S_BOARD *pos, const S_HASHTABLE *table);

void InitHashTable(S_HASHTABLE *table, const int MB);

void ClearHashTable(S_HASHTABLE *table);

void StoreHashEntry(S_BOARD *pos, S_HASHTABLE *table, const int move, int score, const int flags, const int depth);

int ProbeHashEntry(S_BOARD *pos, S_HASHTABLE *table, int *move, int *score, int alpha, int beta, int depth);

int ProbePvMove(const S_BOARD *pos, const S_HASHTABLE *table);

#endif