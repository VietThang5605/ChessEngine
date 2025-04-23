#include "init.h"

#include <iostream>

int Sq120ToSq64[BRD_SQ_NUM];
int Sq64ToSq120[64];

U64 SetMask[64]; //see at init.h
U64 ClearMask[64];

U64 PieceKeys[13][120];
U64 SideKey;
U64 CastleKeys[16];

void InitHashKeys() {
    for (int i = 0; i < 13; ++i) {
        for (int j = 0; j < 120; ++j) {
            PieceKeys[i][j] = RAND_64;
        }
    }
    SideKey = RAND_64;
    for (int i = 0; i < 16; i++) {
        CastleKeys[i] = RAND_64;
    }
}

void InitBitMasks() {
    for (int i = 0; i < SQUARE_NB; ++i) {
        SetMask[i] = (1ULL << i);
        ClearMask[i] = ~SetMask[i];
    }
}

void clearBit(U64 *bb, U64 sq) {
    *bb &= ClearMask[sq]; 
}

void setBit(U64 *bb, U64 sq) {
    *bb |= SetMask[sq];
}

void InitSq120To64(){
    for (int i = 0; i < BRD_SQ_NUM; ++i){
        Sq120ToSq64[i] = 65; //indexing them all to 65 becuz thge max is 63 (0 to 63 cases)
    }

    for (int i = 0; i < SQUARE_NB; ++i){
        Sq64ToSq120[i] = 120; // same as 65 but for the 120 array
    }

    int sq = A1;
    int sq64 = 0;
    
    for(Rank rank = RANK_1; rank <= RANK_8; ++rank){ // we browse through all the ranks
        for(File file = FILE_A; file <= FILE_H; ++file){ // .... the files
            sq = FR2SQ(file, rank); // fpor each coordinate (file, rank), we assign it a square in the 64 array
            Sq64ToSq120[sq64] = sq;
            Sq120ToSq64[sq] = sq64;
            sq64++; // at the end sq64 will represent the 64 base index array for that specific square
        }
    }
}

void InitSrandTime() {
    srand(time(NULL)); // Seed based on current time
}

void AllInit() {
    InitSrandTime(); //must before any of using rand()
    InitSq120To64();
    InitBitMasks();
    InitHashKeys();
}