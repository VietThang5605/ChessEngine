#include "pvtable.h"
#include "board.h"
#include "movegen.h"
#include "makemove.h"

int GetPvLine(const int depth, S_BOARD *pos) {
	ASSERT(depth < MAXDEPTH && depth >= 1);

	int move = ProbePvMove(pos);
	int count = 0;
	
	while (move != NOMOVE && count < depth) { //loop through all the moves until its an illegal move
    //or until we reached the maxdepth
		ASSERT(count < MAXDEPTH);
	
		if (MoveExists(pos, move)) {  // if it exists, we add it in the array
			MakeMove(pos, move);
			pos->PvArray[count++] = move;
		} else {
			break;
		}		
		move = ProbePvMove(pos);	
	}
	
	while (pos->ply > 0) {
		TakeMove(pos);
	}
	
	return count;
}

void InitHashTable(S_HASHTABLE *table, const int MB) {
	int HashSize = 0x100000 * MB;
    table->numEntries = HashSize / sizeof(S_HASHENTRY);
    table->numEntries -= 2;
	
	if (table->pTable != NULL) {
		delete[] table->pTable;
        table->pTable = NULL;
	}

	table->pTable = new S_HASHENTRY[table->numEntries];

	if (table->pTable == NULL) {
		std::cout << "Hash Allocation Failed, trying " << MB / 2 << "...\n";
		InitHashTable(table, MB / 2);
	} else {
		ClearHashTable(table);
		std::cout << "HashTable init complete with " << table->numEntries << " entries\n";
	}
	
}

void ClearHashTable(S_HASHTABLE *table) {
	for (S_HASHENTRY *tableEntry = table->pTable; tableEntry < table->pTable + table->numEntries; ++tableEntry) {
		tableEntry->posKey = 0ULL;
		tableEntry->move = NOMOVE;
		tableEntry->depth = 0;
		tableEntry->score = 0;
		tableEntry->flags = 0;
	}
	table->newWrite = 0;
}

void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth) {
	int i = pos->posKey % pos->HashTable->numEntries;
	
	ASSERT(i >= 0 && i <= pos->HashTable->numEntries - 1);
	ASSERT(depth >= 1 && depth < MAXDEPTH);
    ASSERT(flags >= HFALPHA && flags <= HFEXACT);
    ASSERT(score >= -INF &&score <= INF);
    ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	if (pos->HashTable->pTable[i].posKey == 0) {
		pos->HashTable->newWrite++;
	} else {
		pos->HashTable->overWrite++;
	}
	
	if (score > ISMATE) score += pos->ply;
    else if (score < -ISMATE) score -= pos->ply;
	
	pos->HashTable->pTable[i].move = move;
    pos->HashTable->pTable[i].posKey = pos->posKey;
	pos->HashTable->pTable[i].flags = flags;
	pos->HashTable->pTable[i].score = score;
	pos->HashTable->pTable[i].depth = depth;
}

int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth) {
	int i = pos->posKey % pos->HashTable->numEntries;
	
	ASSERT(i >= 0 && i <= pos->HashTable->numEntries - 1);
    ASSERT(depth >= 1 && depth < MAXDEPTH);
    ASSERT(alpha < beta);
    ASSERT(alpha >= -INF && alpha <= INF);
    ASSERT(beta >= -INF && beta <= INF);
    ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	if (pos->HashTable->pTable[i].posKey == pos->posKey ) {
		*move = pos->HashTable->pTable[i].move;
		if(pos->HashTable->pTable[i].depth >= depth){
			pos->HashTable->hit++;
			
			ASSERT(pos->HashTable->pTable[i].depth >= 1 && pos->HashTable->pTable[i].depth < MAXDEPTH);
            ASSERT(pos->HashTable->pTable[i].flags >= HFALPHA && pos->HashTable->pTable[i].flags <= HFEXACT);
			
			*score = pos->HashTable->pTable[i].score;
			if (*score > ISMATE) *score -= pos->ply;
            else if (*score < -ISMATE) *score += pos->ply;
			
			switch (pos->HashTable->pTable[i].flags) {
                case HFALPHA: if (*score <= alpha) {
						*score=alpha;
						return TRUE;
					}
                    break;
                case HFBETA: if (*score >= beta) {
						*score=beta;
						return TRUE;
                    }
                    break;
                case HFEXACT:
                    return TRUE;
                    break;
                default: ASSERT(FALSE); break;
            }
		}
	}
	
	return FALSE;
}

int ProbePvMove(const S_BOARD *pos) {
	int i = pos->posKey % pos->HashTable->numEntries;
	ASSERT(i >= 0 && i <= pos->HashTable->numEntries - 1);

	if (pos->HashTable->pTable[i].posKey == pos->posKey) {
		return pos->HashTable->pTable[i].move;
	}
	
	return NOMOVE;
}