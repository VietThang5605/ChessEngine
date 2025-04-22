#include "types.h"

#include <stdlib.h>

int Sq120ToSq64[BRD_SQ_NUM]; 
int Sq64ToSq120[64];

void InitSq120To64(){
    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;
    int sq64 = 0;

    for (index = 0; index < BRD_SQ_NUM; index++){
        Sq120ToSq64[index] = 65; //indexing them all to 65 becuz thge max is 63 (0 to 63 cases)
    }

    for (index = 0; index < 64; index ++){
        Sq64ToSq120[index] = 120; // same as 65 but for the 120 array
    }
    
    for(rank = RANK_1; rank <= RANK_8; rank++){ // we browse through all the ranks
        for(file=FILE_A; file <= FILE_H; file++){ // .... the files
            sq = FR2SQ(file, rank); // fpor each coordinate (file, rank), we assign it a square in the 64 array
            Sq64ToSq120[sq64] = sq;
            Sq120ToSq64[sq] = sq64;
            sq64++; // at the end sq64 will represent the 64 base index array for that specific square
        }
    }
}

void AllInit() {
    InitSq120To64();
}