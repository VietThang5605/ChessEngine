#include "types.h"
#include "init.h"
#include "bitboards.h"

#include <iostream>

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

    U64 playBitBoard = 0ULL;
    // for (int i = 0; i < 64; ++i) {
        // cout << "Index: " << i << '\n';
        // PrintBitBoard(SetMask[i]);
        // cout << '\n';

    //     cout << "Index: " << i << '\n';
    //     PrintBitBoard(ClearMask[i]);
    //     cout << '\n';
    // }

    SETBIT(&playBitBoard, 61);
    PrintBitBoard(playBitBoard);

    SETBIT(&playBitBoard, 11);
    PrintBitBoard(playBitBoard);

    CLRBIT(&playBitBoard, 61);
    PrintBitBoard(playBitBoard);

    return 0;
}