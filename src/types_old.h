#ifndef TYPES_OLD_H
#define TYPES_OLD_H

#include <stdlib.h>

// #define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(! (n)) { \
    printf("%s - FAILED ", #n); \
    printf("on %s ", __DATE__); \
    printf("at %s ", __TIME__); \
    printf("In file %s ", __FILE__); \
    printf("At line %d\n", __LINE__); \
    exit(1);}

#endif

typedef unsigned long long U64;

#define NAME ChessEngine
#define BRD_SQ_NUM 120   // cuz 120 cases on the board (0 to 120)

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define MAXGAMEMOVES 2048 // number max of moves in a game (will never never never be more than 2048 exept if u trynna beat a world record)
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 64

#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define INFINITE 30000
#define MATE (INFINITE - MAXDEPTH)

enum Mode {
    UCIMODE, XBOARDMODE, CONSOLEMODE
};

enum Color {
    WHITE, BLACK, BOTH
};

enum Square : int {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD,
    SQUARE_NB = 64
};

enum {
    FALSE, TRUE
};

enum Castling {
    WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8
};

enum File : int {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H,
};

enum Rank : int {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB
};

enum Piece {
    NO_PIECE, 
    wP, wN, wB, wR, wQ, wK,
    bP, bN, bB, bR, bQ, bK,
    PIECE_NB
};

struct S_UNDO {
    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    U64 posKey; //or hashkey its the same
};

struct S_BOARD {
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

    S_UNDO history[MAXGAMEMOVES]; //  where we will store all the moves made dyring the game

    // piece list
    /*we could loop on the entiere board until we come across avery piece and genereate all the moves possible
    but its not very fast, so we do a piece list*/ 

    int pList[PIECE_NB][10]; // 13 different pieeces which u can have maximum 10 each (like promoting all ur pawns to roks and u get 10 rooks)
};

/* MACROS */

#define FR2SQ(f,r) ( (21 + (f) ) + ( (r) * 10) ) // for a given file (f) and rank (r) returns the equivalent square in the 120 square 2D array
#define SQ64(sq120) (Sq120ToSq64[(sq120)])
#define SQ120(sq64) (Sq64ToSq120[(sq64)])

/* GLOBALS */

extern int Sq120ToSq64[BRD_SQ_NUM]; // will convert the index from the 120 squares board (with the grey things) to the "real" board of 64 cases
extern int Sq64ToSq120[64]; // literally the opposite of the previous

/* FUNCTIONS */

// init.c
extern void AllInit();

// bitboards.c
// extern void PrintBitBoard(U64 bb);

#endif