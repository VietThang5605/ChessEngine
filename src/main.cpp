#include "init.h"
#include "board.h"
#include "search.h"
#include "uci.h"
#include "polybook.h"
#include "pvtable.h"
#include "bitboards.h"

#include <iostream>
#include <limits>
#include <cstring>

#define WAC1 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"
#define PERFT "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

int main(int argc, char *argv[]) {
    AllInit();

    S_BOARD board[1];
    S_SEARCHINFO info[1];

    info->quit = FALSE;
	info->threadNumber = 1;
    InitHashTable(HashTable, 64);

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

	// TempHashTest(WAC1);
	// TempHashTest(PERFT);
    
    std::cout << "Welcome to Unstoppable Evaluation Tool (UET)!\n";

	char line[256];
	while (TRUE) {
		if (!std::cin.getline(line, sizeof(line))) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}

		if (!strncmp(line, "uci", 3)) {
			Uci_Loop(board, info);
			if(info->quit == TRUE) break;
			continue;

		} else if (!strncmp(line, "quit", 4)) {
			break;
		}
	}

    CleanPolyBook();
	FreeMagic();
    delete[] HashTable->pTable;

    return 0;
}