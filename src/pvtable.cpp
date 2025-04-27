#include "pvtable.h"
#include "board.h"
#include "movegen.h"
#include "makemove.h"

const int PvSize = 0x100000 * 2;

int GetPvLine(const int depth, S_BOARD *pos) {
	ASSERT(depth < MAXDEPTH && depth >= 1);

	int move = ProbePvTable(pos);
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
		move = ProbePvTable(pos);	
	}
	
	while (pos->ply > 0) {
		TakeMove(pos);
	}
	
	return count;
}

void InitPvTable(S_PVTABLE *table) {
	table->numEntries = PvSize / sizeof(S_PVENTRY);
	table->numEntries -= 2;

    if (table->pTable != NULL) {
	    // free(table->pTable);
        delete[] table->pTable;
        table->pTable = NULL;
    }

	// table->pTable = (S_PVENTRY *) malloc(table->numEntries * sizeof(S_PVENTRY));
    table->pTable = new S_PVENTRY[table->numEntries];

	ClearPvTable(table);
    std::cout << "PvTable init complete with " << table->numEntries << " entries \n";
	//printf("PvTable init complete with %d entries \n", table->numEntries);
}

void ClearPvTable(S_PVTABLE *table) {
	for (S_PVENTRY *pvEntry = table->pTable; pvEntry < table->pTable + table->numEntries; pvEntry++) {
		pvEntry->posKey = 0ULL;
		pvEntry->move = NOMOVE;
	}
}

void StorePvMove(const S_BOARD *pos, const int move) {
	int i = pos->posKey % pos->PvTable->numEntries;
	ASSERT(i >= 0 && i <= pos->PvTable->numEntries - 1);
	pos->PvTable->pTable[i].move = move;
	pos->PvTable->pTable[i].posKey = pos->posKey;
}

int ProbePvTable(const S_BOARD *pos) {
	int i = pos->posKey % pos->PvTable->numEntries;
	ASSERT(i >= 0 && i <= pos->PvTable->numEntries - 1);

	if (pos->PvTable->pTable[i].posKey == pos->posKey) {
		return pos->PvTable->pTable[i].move;
	}

	return NOMOVE;
}