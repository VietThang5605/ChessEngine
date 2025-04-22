#include "types.h"

#include <iostream>

void PrintBitBoard(U64 bb){
    U64 shiftMe = 1ULL; //long long cuz we need more than 32 bits

    int rank =0;
    int file = 0;
    int sq = 0;
    int sq64 = 0;

    std::cout << '\n';
    for(rank = RANK_8; rank >= RANK_1; --rank){ //starting at the 8th rank and file A
        for (file = FILE_A; file <= FILE_H; ++file){ //so on to 1st rank and file H
            sq = FR2SQ(file, rank); // the 120 base index
            sq64 = SQ64(sq); // need the 64 base index cuz we gon use our 64 bit number
            if((shiftMe << sq64) & bb) //so if there's a bit on there, print ax X
                std::cout << 'X';
            else   
                std::cout << '-'; //otherwise print a minus
        }
        std::cout << '\n'; 
    }
    std::cout << '\n';
}