#include "pvtable.h"
#include "board.h"
#include "movegen.h"
#include "makemove.h"

S_HASHTABLE HashTable[1];

void DataCheck(const int move) {
	int depth = rand() % MAXDEPTH;
	int flag = rand() % 3;
	int score = rand() % AB_BOUND;

	U64 data = FOLD_DATA(score, depth, flag, move);
	std::cout << "Oringinal move: " << PrintMove(move) 
			<< " depth: " << depth 
			<< " flag:" << flag 
			<< " score:" << score 
			<< " data:" << data 
			<< '\n';

	std::cout << "Check move: " << PrintMove(EXTRACT_MOVE(data))
			<< " depth: " << EXTRACT_DEPTH(data) 
			<< " flag:" << EXTRACT_FLAGS(data) 
			<< " score:" << EXTRACT_SCORE(data)
			<< " data:" << data << '\n';		
}

void TempHashTest(char *fen) {
	S_BOARD board[1];
	ParseFen(fen, board);

	S_MOVELIST list[1];
    GenerateAllMoves(board, list);

    for (int MoveNum = 0; MoveNum < list->count; ++MoveNum) {	
        if (!MakeMove(board, list->moves[MoveNum].move)) {
            continue;
        }
        TakeMove(board);
		DataCheck(list->moves[MoveNum].move);
    }
}

void VerifyEntrySMP(S_HASHENTRY *entry) {
	U64 data = FOLD_DATA(entry->score, entry->depth, entry->flags, entry->move);
	U64 key = entry->posKey ^ data;

	if (data != entry->smp_data) { printf("data error"); exit(1);}
	if (key != entry->smp_key) { printf("smp_key error"); exit(1);}

	int move = EXTRACT_MOVE(data);
	int flag = EXTRACT_FLAGS(data);
	int score = EXTRACT_SCORE(data);
	int depth = EXTRACT_DEPTH(data);
	
	if (move != entry->move) { printf("move error"); exit(1);}
	if (flag != entry->flags) { printf("flags error"); exit(1);}
	if (score != entry->score) { printf("score error"); exit(1);}
	if (depth != entry->depth) { printf("depth error"); exit(1);}
}

int GetPvLine(const int depth, S_BOARD *pos, const S_HASHTABLE *table) {
	ASSERT(depth < MAXDEPTH && depth >= 1);

	int move = ProbePvMove(pos, table);
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
		move = ProbePvMove(pos, table);	
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
	for (S_HASHENTRY *tableEntry = table->pTable; tableEntry < table->pTable + table->numEntries; tableEntry++) {
		tableEntry->posKey = 0ULL;
		tableEntry->move = NOMOVE;
		tableEntry->depth = 0;
		tableEntry->score = 0;
		tableEntry->flags = 0;
		tableEntry->age = 0;
		tableEntry->smp_key = 0ULL;
		tableEntry->smp_data = 0ULL;
	}
	table->newWrite = 0;
	table->currentAge = 0;
}

void StoreHashEntry(S_BOARD *pos, S_HASHTABLE *table, const int move, int score, const int flags, const int depth) {
	int i = pos->posKey % table->numEntries;
	
	ASSERT(i >= 0 && i <= table->numEntries - 1);
	ASSERT(depth >= 1 && depth < MAXDEPTH);
    ASSERT(flags >= HFALPHA && flags <= HFEXACT);
    ASSERT(score >= -AB_BOUND && score <= AB_BOUND);
    ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);

	bool replace = FALSE;
	
	if (table->pTable[i].posKey == 0) {
		table->newWrite++;
		replace = TRUE;
	} else {
		if (table->pTable[i].age < table->currentAge || 
			table->pTable[i].depth <= depth) {
					replace = TRUE;
		}
	}

	if (replace == FALSE) return;
	
	if (score > ISMATE) score += pos->ply;
    else if (score < -ISMATE) score -= pos->ply;

	U64 smp_data = FOLD_DATA(score, depth, flags, move);
	U64 smp_key = pos->posKey ^ smp_data;
	
	table->pTable[i].move = move;
    table->pTable[i].posKey = pos->posKey;
	table->pTable[i].flags = flags;
	table->pTable[i].score = score;
	table->pTable[i].depth = depth;
	table->pTable[i].age = table->currentAge;
	table->pTable[i].smp_key = smp_key;
	table->pTable[i].smp_data = smp_data;

	VerifyEntrySMP(&table->pTable[i]);
}

int ProbeHashEntry(S_BOARD *pos, S_HASHTABLE *table, int *move, int *score, int alpha, int beta, int depth) {
	int i = pos->posKey % table->numEntries;
	
	ASSERT(i >= 0 && i <= table->numEntries - 1);
    ASSERT(depth >= 1 && depth < MAXDEPTH);
    ASSERT(alpha < beta);
    ASSERT(alpha >= -AB_BOUND && alpha <= AB_BOUND);
    ASSERT(beta >= -AB_BOUND && beta <= AB_BOUND);
    ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	if (table->pTable[i].posKey == pos->posKey ) {
		U64 test_key = table->pTable[i].posKey ^ table->pTable[i].smp_data;
		if (test_key != table->pTable[i].smp_key) {
			std::cout << "Error test_key\n";
		}
		VerifyEntrySMP(&table->pTable[i]);

		*move = table->pTable[i].move;
		if(table->pTable[i].depth >= depth){
			table->hit++;
			
			ASSERT(table->pTable[i].depth >= 1 && table->pTable[i].depth < MAXDEPTH);
            ASSERT(table->pTable[i].flags >= HFALPHA && table->pTable[i].flags <= HFEXACT);
			
			*score = table->pTable[i].score;
			if (*score > ISMATE) *score -= pos->ply;
            else if (*score < -ISMATE) *score += pos->ply;
			
			switch (table->pTable[i].flags) {
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

int ProbePvMove(const S_BOARD *pos, const S_HASHTABLE *table) {
	int i = pos->posKey % table->numEntries;
	ASSERT(i >= 0 && i <= table->numEntries - 1);

	if (table->pTable[i].posKey == pos->posKey) {
		return table->pTable[i].move;
	}
	
	return NOMOVE;
}