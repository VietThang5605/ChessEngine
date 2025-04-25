#include "io.h"

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

void PrintMoveList(const S_MOVELIST *list) {
	std::cout << "MoveList:\n";
	for(int i = 0; i < list->count; ++i) {
		int move = list->moves[i].move;
		int score = list->moves[i].score;
		std::cout << "Move: " << i + 1 << " > " << PrintMove(move) << " (score:" << score << ")\n";
	}
	std::cout << "MoveList Total " << list->count << "  Moves\n\n";
}