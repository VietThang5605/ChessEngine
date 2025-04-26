#include "io.h"
#include "init.h"
#include "data.h"
#include "validate.h"
#include "movegen.h"

char *PrintSquare(const int sq) {
    static char SquareStr[3]; //e4 - the last char is for \0

    int file = FilesBrd[sq]; 
	int rank = RanksBrd[sq];

	// sprintf(SquareStr, "%c%c", ('a'+file), ('1'+rank));
    snprintf(SquareStr, sizeof(SquareStr), "%c%c", ('a'+file), ('1'+rank));

	return SquareStr;
}

char *PrintMove(const int move) {
	static char MoveStr[6]; //e7e8q and \0 - from square, to square and promoted (if have)

	int fromFile = FilesBrd[FROMSQ(move)];
	int fromRank = RanksBrd[FROMSQ(move)];
	int toFile = FilesBrd[TOSQ(move)];
	int toRank = RanksBrd[TOSQ(move)];

	int promoted = PROMOTED(move);

    if (promoted) { // if we promote a pawn
		char pieceChar = 'q'; // it is a queen (most of the time)
		if (IsKnight(promoted)) { // else we change it
			pieceChar = 'n';
		} else if (IsRookQueen(promoted) && !IsBishopQueen(promoted)) {
			pieceChar = 'r';
		} else if (!IsRookQueen(promoted) && IsBishopQueen(promoted)) {
			pieceChar = 'b';
		}
		snprintf(MoveStr, sizeof(MoveStr), "%c%c%c%c%c", ('a'+fromFile), ('1'+fromRank), ('a'+toFile), ('1'+toRank), pieceChar);
	} else {
		snprintf(MoveStr, sizeof(MoveStr), "%c%c%c%c", ('a'+fromFile), ('1'+fromRank), ('a'+toFile), ('1'+toRank));
	}

	return MoveStr;
}

int ParseMove(char *ptrChar, S_BOARD *pos) {
	ASSERT(CheckBoard(pos));

	if(ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE;
    if(ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;
    if(ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;
    if(ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;

    int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
    int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

	std::cout << "prtChar:" << ptrChar << " from:" << from << " to:" << to << '\n';

	ASSERT(SqOnBoard(from) && SqOnBoard(to));

	S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

	for (int MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		int Move = list->moves[MoveNum].move;
		if (FROMSQ(Move) == from && TOSQ(Move) == to) {
			int PromotedPiece = PROMOTED(Move);
			if (PromotedPiece != EMPTY) {
				if (IsRookQueen(PromotedPiece) && !IsBishopQueen(PromotedPiece) && ptrChar[4] == 'r') {
					return Move;
				} else if (!IsRookQueen(PromotedPiece) && IsBishopQueen(PromotedPiece) && ptrChar[4] == 'b') {
					return Move;
				} else if (IsRookQueen(PromotedPiece) && IsBishopQueen(PromotedPiece) && ptrChar[4] == 'q') {
					return Move;
				} else if (IsKnight(PromotedPiece)&& ptrChar[4] == 'n') {
					return Move;
				}
				continue;
			}
			return Move;
		}
    }

    return NOMOVE;
}

void PrintMoveList(const S_MOVELIST *list) {
	std::cout << "MoveList:\n";
	for(int i = 0; i < list->count; ++i) {
		int move = list->moves[i].move;
		int score = list->moves[i].score;
		std::cout << "Move: " << i + 1 << " > " << PrintMove(move) << " (score:" << score << ")\n";
	}
	std::cout << "MoveList Total " << list->count << "  Moves\n\n";
}