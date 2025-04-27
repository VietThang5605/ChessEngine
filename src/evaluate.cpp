#include "evaluate.h"
#include "validate.h"
#include "init.h"

int EvalPosition(const S_BOARD *pos) {
    int piece;
	int sq;
	int score = pos->material[WHITE] - pos->material[BLACK];

    piece = wP;	
	for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
		sq = pos->pieceList[piece][pieceNum];
		ASSERT(SqOnBoard(sq));
		score += PawnTable[SQ64(sq)];	
	}	

	piece = bP;	
	for(int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
		sq = pos->pieceList[piece][pieceNum];
		ASSERT(SqOnBoard(sq));
		score -= PawnTable[MIRROR64(SQ64(sq))];		  
	}

    piece = wN;	
	for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
		sq = pos->pieceList[piece][pieceNum];
		ASSERT(SqOnBoard(sq));
		score += KnightTable[SQ64(sq)];
	}	

	piece = bN;	
	for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
		sq = pos->pieceList[piece][pieceNum];
		ASSERT(SqOnBoard(sq));
		score -= KnightTable[MIRROR64(SQ64(sq))];
	}			
	
	piece = wB;	
	for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
		sq = pos->pieceList[piece][pieceNum];
		ASSERT(SqOnBoard(sq));
		score += BishopTable[SQ64(sq)];
	}	

	piece = bB;	
	for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
		sq = pos->pieceList[piece][pieceNum];
		ASSERT(SqOnBoard(sq));
		score -= BishopTable[MIRROR64(SQ64(sq))];
	}	

	piece = wR;	
	for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
		sq = pos->pieceList[piece][pieceNum];
		ASSERT(SqOnBoard(sq));
		score += RookTable[SQ64(sq)];
	}	

	piece = bR;	
	for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
		sq = pos->pieceList[piece][pieceNum];
		ASSERT(SqOnBoard(sq));
		score -= RookTable[MIRROR64(SQ64(sq))];

	}

    std::cout << "Found a " << piece << " on " << sq << " score: " << score << '\n';
	if (pos->side == WHITE) {
		return score;
	} else {
		return -score;
	}
}