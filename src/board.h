#pragma once

#include "types.h"

struct S_Undo {
    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    U64 posKey; //or hashkey its the same
};

struct S_Board {
    int pieces[BRD_SQ_NUM];
    U64 pawns[3]; // 0100000 for the first line means we have a pawn on B1

    int KingSq[2]; // same for kings

    int side;
    int enPas;
    int fiftyMove;

    int ply;
    int hisPly;

    int castlePerm; // will be represented by 4 bits 0(WKCA) 0(WQCA) 0(BKCA) 0(BQCA) so the castle permission
                    // if we have  1 0 0 1, we can castle king side for whites & queen side for blacks

    U64 posKey; //or hashkey again will be used to represent the position of the board

    int pceNum[13]; // number of different pieces on the board ( pawn, bishop, rooq, knicght, queen, king) x2 for black and white and then a empty case
    int bigPce[3]; // number of "big" pieces (everything that's not a pawn)
    int majPce[3]; // major pieces (queen and rooqs)
    int minPce[3]; // minor pieces (knight and bishop)
    int material[2];

    // piece list
    /*we could loop on the entiere board until we come across avery piece and genereate all the moves possible
    but its not very fast, so we do a piece list*/ 

    int pList[PIECE_NB][10]; // 13 different pieeces which u can have maximum 10 each (like promoting all ur pawns to roks and u get 10 rooks)

    
    //bitboards for each piece type
    U64 knightsBB[2]; // 2 for black and white
    U64 bishopsBB[2]; // 2 for black and white
    U64 rooksBB[2]; // 2 for black and white
    U64 queensBB[2]; // 2 for black and white
    U64 kingsBB[2]; // 2 for black and white

     // Bitboards cho tất cả các quân của một màu, và cả hai màu
    // Quy ước chỉ số mảng: [WHITE = 0], [BLACK = 1], [BOTH = 2] (giống pawns[3])
    U64 all_piecesBB[3];
};