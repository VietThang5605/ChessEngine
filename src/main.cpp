#include "init.h"
#include "board.h"
#include "search.h"
#include "uci.h"
#include "polybook.h"
#include "pvtable.h"

#include <iostream>
#include <limits>
#include <cstring>

int main(int argc, char *argv[]) {
    AllInit();

    S_BOARD board[1];
    S_SEARCHINFO info[1];

    info->quit = FALSE;
    InitHashTable(HashTable, 64);

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    
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
    delete[] HashTable->pTable;

    return 0;
}