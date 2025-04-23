#include "types.h"
#include "init.h"
#include "bitboards.h"
#include "board.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define FEN4 "r1bqk2r/ppp2ppp/2n2n2/3pp1B1/1bB1P3/2NP1N2/PPP2PPP/R2QK2R b KQkq - 1 6"

using namespace std;

int main() {
    AllInit();

    // int num = 2, nuts = 4;
    // ASSERT(num == nuts);

    //Part 7
    // for (int index = 0; index < BRD_SQ_NUM; index++) {
    //     if (index % 10 == 0)
    //         std::cout << '\n';
    //     printf("%5d", Sq120ToSq64[index]);
    // }

    // std::cout << '\n';
    // std::cout << '\n';

    // for (int index = 0; index < 64; ++index) {
    //     if (index % 8 == 0)
    //         std::cout << '\n';
    //     printf("%5d", Sq64ToSq120[index]);
    // }

    //Part 8

    // U64 playBitBoard = 0ULL;

    // cout << "Start:\n";
    // PrintBitBoard(playBitBoard);

    // playBitBoard |= (1ULL << SQ64(D2));
    // cout << "D2 added:\n";
    // PrintBitBoard(playBitBoard);

    // playBitBoard |= (1ULL << SQ64(B7));
    // cout << "B7 added:\n";
    // PrintBitBoard(playBitBoard);

    // playBitBoard |= (1ULL << SQ64(G7));
    // cout << "G7 added:\n";
    // PrintBitBoard(playBitBoard);

    // playBitBoard |= (1ULL << SQ64(F6));
    // cout << "F6 added:\n";
    // PrintBitBoard(playBitBoard);

    // playBitBoard |= (1ULL << SQ64(H4));
    // cout << "H4 added:\n";
    // PrintBitBoard(playBitBoard);

    //Part9
    // U64 playBitBoard = 0ULL;

    // playBitBoard |= (1ULL << SQ64(D2));
    // playBitBoard |= (1ULL << SQ64(D3));
    // playBitBoard |= (1ULL << SQ64(D4));

    // cout << "D2: " << SQ64(D2) << '\n';
    // PrintBitBoard(playBitBoard);

    // int count =  CNT(playBitBoard);
    // cout << "Count: " << count << '\n';

    // int index = POP(&playBitBoard);
    // cout << "Index: " << index << '\n';
    // PrintBitBoard(playBitBoard);

    // count = CNT(playBitBoard);
    // cout << "Count: " << count << '\n';

    // int sq64 = 0;
    // while (playBitBoard) {
    //     sq64 = POP(&playBitBoard);
    //     cout << "Popped: " << sq64 << '\n';
    //     PrintBitBoard(playBitBoard);
    // }

    //Part 10

    // U64 playBitBoard = 0ULL;
    // for (int i = 0; i < 64; ++i) {
        // cout << "Index: " << i << '\n';
        // PrintBitBoard(SetMask[i]);
        // cout << '\n';

    //     cout << "Index: " << i << '\n';
    //     PrintBitBoard(ClearMask[i]);
    //     cout << '\n';
    // }

    // SETBIT(&playBitBoard, 61);
    // PrintBitBoard(playBitBoard);

    // SETBIT(&playBitBoard, 11);
    // PrintBitBoard(playBitBoard);

    // CLRBIT(&playBitBoard, 61);
    // PrintBitBoard(playBitBoard);

    //Part 11
    // int PieceOne = rand();
    // int PieceTwo = rand();
    // int PieceThree = rand();
    // int PieceFour = rand();

    // cout << hex << uppercase << PieceOne << '\n'; 
    // cout << hex << uppercase << PieceTwo << '\n'; 
    // cout << hex << uppercase << PieceThree << '\n'; 
    // cout << hex << uppercase << PieceFour << '\n'; 

    // int Key = PieceOne ^ PieceTwo ^ PieceFour;
    // int TempKey = PieceTwo;
    // TempKey ^= PieceThree;
    // TempKey ^= PieceFour;
    // TempKey ^= PieceOne;

    // cout << "Key: " << hex << uppercase << Key << '\n';
    // cout << "TempKey: " << hex << uppercase << TempKey << '\n';
    // cout << '\n';

    // TempKey ^= PieceThree;
    // cout << "(Three out) TempKey: " << hex << uppercase << TempKey << '\n';

    // TempKey ^= PieceThree;
    // cout << "(Three in again) TempKey: " << hex << uppercase << TempKey << '\n';

    //Part 12
    // for (int i = 1; i <= 10; ++i)
    //     cout << RAND_15 << ' ' << RAND_64 << '\n';

    //Part 13
    // S_BOARD board;
    // ResetBoard(&board);

    //Part 17
    S_BOARD board[1];

    ParseFen(START_FEN, board);
    PrintBoard(board);

    ParseFen(FEN1, board);
    PrintBoard(board);
    
    ParseFen(FEN2, board);
    PrintBoard(board);

    ParseFen(FEN3, board);
    PrintBoard(board);

    ParseFen(FEN4, board);
    PrintBoard(board);
    return 0;
}