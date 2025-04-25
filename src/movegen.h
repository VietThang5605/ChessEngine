#include "types.h"
#include "board.h"
#include "validate.h"

#define MOVE(from, to, captured, promoted, fl) ((from) | ((to) << 7) | ((captured) << 14) | ((promoted) << 20) | (fl))
#define SQOFFBOARD(sq) (FilesBrd[(sq)] == OFFBOARD)

void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list);

void AddCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *list);

void AddEnPassantMove(const S_BOARD *pos, int move, S_MOVELIST *list);

void AddWhitePawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list);

void AddWhitePawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list);

void AddBlackPawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list);

void AddBlackPawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list);

void GenerateAllMove(const S_BOARD *pos, S_MOVELIST *list);