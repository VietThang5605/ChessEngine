#ifndef BOARD_H
#define BOARD_H

#include "types.h"
#include "init.h"
#include "hashkeys.h"
#include "data.h"
#include "bitboards.h"

struct S_UNDO
{
    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    U64 posKey; // or hashkey its the same
};

struct S_BOARD
{
    int pieces[BRD_SQ_NUM];
    U64 pawnsBB[3]; // 0100000 for the first line means we have a pawn on B1
    U64 knightsBB[3];
    U64 bishopsBB[3];
    U64 rooksBB[3];
    U64 queensBB[3];
    U64 kingsBB[3];
    U64 allPiecesBB[3];

    int kingSquare[2]; // same for kings

    int side;
    int enPas;
    int fiftyMove;

    int ply;
    int hisPly;

    int castlePerm; // will be represented by 4 bits 0(WKCA) 0(WQCA) 0(BKCA) 0(BQCA) so the castle permission
                    // if we have  1 0 0 1, we can castle king side for whites & queen side for blacks

    U64 posKey; // or hashkey again will be used to represent the position of the board

    int pieceNum[13];  // number of different pieces on the board ( pawn, bishop, rooq, knicght, queen, king) x2 for black and white and then a empty case
    int bigPiece[3];   // number of "big" pieces (everything that's not a pawn)
    int majPiece[3];   // major pieces (queen and rooqs)
    int minPiece[3];   // minor pieces (knight and bishop)
    int bigPiece[3];   // number of "big" pieces (everything that's not a pawn)
    int majorPiece[3]; // major pieces (queen and rooqs)
    int minorPiece[3]; // minor pieces (knight and bishop)
    int material[2];

    S_UNDO history[MAXGAMEMOVES];

    // piece list
    /*we could loop on the entiere board until we come across avery piece and genereate all the moves possible
    but its not very fast, so we do a piece list*/
    int pieceList[PIECE_NB][10]; // 13 different pieces which u can have maximum 10 each (like promoting all ur pawns to roks and u get 10 rooks)

    int searchHistory[13][BRD_SQ_NUM];
    int searchKillers[2][MAXDEPTH];
};

bool CheckBoard(const S_BOARD *pos);

void UpdateListsMaterial(S_BOARD *pos);

int ParseFen(char *fen, S_BOARD *pos);

void ResetBoard(S_BOARD *pos);

void PrintBoard(const S_BOARD *pos);

#endif