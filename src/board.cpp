#include "board.h"

#include <iostream>

void ResetBoard(S_BOARD *pos) {
    for(int i = 0; i < BRD_SQ_NUM; ++i){ //looping through all 120 squares
        pos->pieces[i] = OFFBOARD; // and setting them to "OFFBOARD"
    } //for more understanding, look at the image in the folder named "board representation"

    for(int i = 0; i < SQUARE_NB; ++i) {
        pos->pieces[SQ120(i)] = EMPTY; // then the "real" squares are set to "empty"
    }

    for(int i = 0; i < 2; ++i) {
        pos->bigPiece[i] = 0; // number of all pieces (black and white)
        pos->majPiece[i] = 0;
        pos->minPiece[i] = 0;
        pos->material[i] = 0; //set material scores to 0
    }

    for(int i = 0; i < 3; i++) {
        pos->pawnsBB[i] = 0ULL; // OULL cuz pawnsBB are an U64 type in an unsigned long long
        pos->knightsBB[i] = 0ULL;
        pos->bishopsBB[i] = 0ULL;
        pos->rooksBB[i] = 0ULL;
        pos->queensBB[i] = 0ULL;
        pos->kingsBB[i] = 0ULL;
    }

    for(int i = 0; i < 13; ++i) {
        pos->pieceNum[i] = 0; //resets the piece number
    }

    pos->KingSq[WHITE] = pos->KingSq[BLACK] = NO_SQ;

    pos->side = BOTH; //once we setup a position, checkboard will check some things so we set it to both not to get an ASSERT
    pos->enPas = NO_SQ; 
    pos->fiftyMove = 0;

    pos->ply = 0; //nber of half moves played in the current search
    pos->hisPly = 0;

    pos->castlePerm = 0;

    pos->posKey = 0ULL;
}