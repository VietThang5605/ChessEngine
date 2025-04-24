#ifndef TYPES_H
#define TYPES_H

#include <cstdlib>
#include <iostream>

#define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(! (n)) { \
    std::cerr << #n << " - FAILED "; \
    std::cerr << "on " << __DATE__ << " "; \
    std::cerr << "at " << __TIME__ << " "; \
    std::cerr << "In file " << __FILE__ << " "; \
    std::cerr << "At line " << __LINE__ << std::endl; \
    exit(1); \
}

// printf("%s - FAILED ", #n); \
// printf("on %s ", __DATE__); \
// printf("at %s ", __TIME__); \
// printf("In file %s ", __FILE__); \
// printf("At line %d\n", __LINE__);

#endif

// typedef unsigned long long U64;
typedef uint64_t U64;

#define NAME ChessEngine
#define BRD_SQ_NUM 120   // cuz 120 cases on the board (0 to 120)

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define MAXGAMEMOVES 2048 // number max of moves in a game (will never never never be more than 2048 exept if u trynna beat a world record)
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 64

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
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB
};

enum Rank : int {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB
};

enum Piece {
    EMPTY,
    wP, wN, wB, wR, wQ, wK,
    bP, bN, bB, bR, bQ, bK,
    PIECE_NB
};

/* MACROS */

#define FR2SQ(f,r) ( (21 + (f) ) + ( (r) * 10) ) // for a given file (f) and rank (r) returns the equivalent square in the 120 square 2D array

/*
0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
0000 0000 0000 0011 1111 1000 0000 -> To >> 7, 0x7F
0000 0000 0011 1100 0000 0000 0000 -> Captured >> 14, 0xF
0000 0000 0100 0000 0000 0000 0000 -> EP 0x40000
0000 0000 1000 0000 0000 0000 0000 -> Pawn Start 0x80000
0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece >> 20, 0xF
0001 0000 0000 0000 0000 0000 0000 -> Castle 0x1000000
*/
#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m) (((m) >> 7) & 0x7F) //for move
#define CAPTURED(m) (((m) >> 14) & 0xF) //the piece captured in a move
#define PROMOTED(m) (((m) >> 20) & 0xF)

#define MOVEFLAG_EP 0x4000
#define MOVEFLAG_PAWNSTART 0x80000
#define MOVEFLAG_CASTLE 0x1000000

#define MOVEFLAG_CAPTURED 0x7C000
#define MOVEFLAG_PROMOTED 0xF00000

/* FUNCTIONS */

#define ENABLE_BASE_OPERATORS_ON(T)                                \
constexpr T operator+(T d1, T d2) { return T(int(d1) + int(d2)); } \
constexpr T operator-(T d1, T d2) { return T(int(d1) - int(d2)); } \
constexpr T operator-(T d) { return T(-int(d)); }                  \
inline T& operator+=(T& d1, T d2) { return d1 = d1 + d2; }         \
inline T& operator-=(T& d1, T d2) { return d1 = d1 - d2; }

#define ENABLE_INCR_OPERATORS_ON(T)                                \
inline T& operator++(T& d) { return d = T(int(d) + 1); }           \
inline T& operator--(T& d) { return d = T(int(d) - 1); }

#define ENABLE_FULL_OPERATORS_ON(T)                                \
ENABLE_BASE_OPERATORS_ON(T)                                        \
constexpr T operator*(int i, T d) { return T(i * int(d)); }        \
constexpr T operator*(T d, int i) { return T(int(d) * i); }        \
constexpr T operator/(T d, int i) { return T(int(d) / i); }        \
constexpr int operator/(T d1, T d2) { return int(d1) / int(d2); }  \
inline T& operator*=(T& d, int i) { return d = T(int(d) * i); }    \
inline T& operator/=(T& d, int i) { return d = T(int(d) / i); }

ENABLE_INCR_OPERATORS_ON(File)
ENABLE_INCR_OPERATORS_ON(Rank)
ENABLE_INCR_OPERATORS_ON(Piece)

#undef ENABLE_FULL_OPERATORS_ON
#undef ENABLE_INCR_OPERATORS_ON
#undef ENABLE_BASE_OPERATORS_ON
#endif