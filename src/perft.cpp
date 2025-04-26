#include "perft.h"

long long leafNodes;

void Perft(int depth, S_BOARD *pos) {
    ASSERT(CheckBoard(pos));

    if (depth == 0) {
        leafNodes++;
        return;
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    for (int MoveNum = 0; MoveNum < list->count; ++MoveNum) {	
        if (!MakeMove(pos, list->moves[MoveNum].move)) {
            continue;
        }
        Perft(depth - 1, pos);
        TakeMove(pos);
    }
}

void PerftTest(int depth, S_BOARD *pos) { //kindof a mirror of the perft function
    ASSERT(CheckBoard(pos));
	PrintBoard(pos);
    
	printf("\nStarting Test To Depth:%d\n",depth);	

	leafNodes = 0;
	// int start = GetTimeMs();

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);	
    
	for (int MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        int move = list->moves[MoveNum].move;
        if (!MakeMove(pos, move)) {
            continue;
        }
        long cumnodes = leafNodes; // create a cumulative node variables on each loop
        Perft(depth - 1, pos);
        TakeMove(pos);        
        long oldnodes = leafNodes - cumnodes;
        std::cout << "move " << MoveNum + 1 << " : " << PrintMove(move) << " : " << oldnodes << '\n';
    }   
	
    std::cout << "\nTest Complete : " << leafNodes << " nodes visited\n";
	// printf("\nTest Complete : %ld nodes visited in %dms\n",leafNodes,GetTimeMs() - start);
    //just shows all the leafs possible
    return;
}