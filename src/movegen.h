#include "types.h"
#include "board.h"
#include "validate.h"
#include "io.h"

#define MOVE(from, to, captured, promoted, fl) ((from) | ((to) << 7) | ((captured) << 14) | ((promoted) << 20) | (fl))
#define SQOFFBOARD(sq) (FilesBrd[(sq)] == OFFBOARD)

const int LoopSlidePiece[8] = {
    wB, wR, wQ, 0, bB, bR, bQ, 0
};

const int LoopNonSlidePiece[6] = {
    wN, wK, 0, bN, bK, 0
};

const int LoopSlideIndex[2] = { 0, 4 };
const int LoopNonSlideIndex[2] = { 0, 3 };

const int PieceDir[13][8] = {
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

const int NumDir[13] = {
    0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};

void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list);

void AddCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *list);

void AddEnPassantMove(const S_BOARD *pos, int move, S_MOVELIST *list);

void AddWhitePawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list);

void AddWhitePawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list);

void AddBlackPawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list);

void AddBlackPawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list);

void GenerateAllMove(const S_BOARD *pos, S_MOVELIST *list);