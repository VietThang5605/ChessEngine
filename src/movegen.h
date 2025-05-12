#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "types.h"
#include "board.h"

#define MOVE(from, to, captured, promoted, fl) ((from) | ((to) << 7) | ((captured) << 14) | ((promoted) << 20) | (fl))
#define SQOFFBOARD(sq) (FilesBrd[(sq)] == OFFBOARD)

constexpr int LoopSlidePiece[8] = {
    wB, wR, wQ, 0, bB, bR, bQ, 0
};

constexpr int LoopNonSlidePiece[6] = {
    wN, wK, 0, bN, bK, 0
};

constexpr int LoopSlideIndex[2] = { 0, 4 };
constexpr int LoopNonSlideIndex[2] = { 0, 3 };

constexpr int PieceDir[13][8] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,  -21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 }
};

constexpr int NumDir[13] = {
    0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};

constexpr int VictimScore[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };

static int MvvLvaScores[13][13];

void InitMvvLva();

int MoveExists(S_BOARD *pos, const int move);

static void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list);

static void AddCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *list);

static void AddEnPassantMove(const S_BOARD *pos, int move, S_MOVELIST *list);

static void AddWhitePawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list);

static void AddWhitePawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list);

static void AddBlackPawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list);

static void AddBlackPawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list);

void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list);

void GenerateAllCaptures(const S_BOARD *pos, S_MOVELIST *list);
#endif