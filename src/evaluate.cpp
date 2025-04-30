#include "evaluate.h"
#include "validate.h"
#include "init.h"
#include "data.h"
#include "io.h"

// sjeng 11.2
//8/6R1/2k5/6P1/8/8/4nP2/6K1 w - - 1 41 
int MaterialDraw(const S_BOARD *pos) {
	ASSERT(CheckBoard(pos));
	
    if (!pos->pieceNum[wR] && !pos->pieceNum[bR] && !pos->pieceNum[wQ] && !pos->pieceNum[bQ]) {
	  if (!pos->pieceNum[bB] && !pos->pieceNum[wB]) {
	      if (pos->pieceNum[wN] < 3 && pos->pieceNum[bN] < 3) {  return TRUE; }
	  } else if (!pos->pieceNum[wN] && !pos->pieceNum[bN]) {
	     if (abs(pos->pieceNum[wB] - pos->pieceNum[bB]) < 2) { return TRUE; }
	  } else if ((pos->pieceNum[wN] < 3 && !pos->pieceNum[wB]) || (pos->pieceNum[wB] == 1 && !pos->pieceNum[wN])) {
	    if ((pos->pieceNum[bN] < 3 && !pos->pieceNum[bB]) || (pos->pieceNum[bB] == 1 && !pos->pieceNum[bN]))  { return TRUE; }
	  }
	} else if (!pos->pieceNum[wQ] && !pos->pieceNum[bQ]) {
        if (pos->pieceNum[wR] == 1 && pos->pieceNum[bR] == 1) {
            if ((pos->pieceNum[wN] + pos->pieceNum[wB]) < 2 && (pos->pieceNum[bN] + pos->pieceNum[bB]) < 2)	{ return TRUE; }
        } else if (pos->pieceNum[wR] == 1 && !pos->pieceNum[bR]) {
            if ((pos->pieceNum[wN] + pos->pieceNum[wB] == 0) && (((pos->pieceNum[bN] + pos->pieceNum[bB]) == 1) || ((pos->pieceNum[bN] + pos->pieceNum[bB]) == 2))) { return TRUE; }
        } else if (pos->pieceNum[bR] == 1 && !pos->pieceNum[wR]) {
            if ((pos->pieceNum[bN] + pos->pieceNum[bB] == 0) && (((pos->pieceNum[wN] + pos->pieceNum[wB]) == 1) || ((pos->pieceNum[wN] + pos->pieceNum[wB]) == 2))) { return TRUE; }
        }
    }
  return FALSE;
}

#define ENDGAME_MATERIAL (1 * PieceValue[wR] + 2 * PieceValue[wN] + 2 * PieceValue[wP] + PieceValue[wK])

int EvalPosition(const S_BOARD *pos) {
    int piece;
	int sq;
	int score = pos->material[WHITE] - pos->material[BLACK];

	if (!pos->pieceNum[wP] && !pos->pieceNum[bP] && MaterialDraw(pos) == TRUE) {
		return 0;
	}

    piece = wP;	
	for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
		sq = pos->pieceList[piece][pieceNum];
		ASSERT(SqOnBoard(sq));
		score += PawnTable[SQ64(sq)];

		if ((IsolatedMask[SQ64(sq)] & pos->pawnsBB[WHITE]) == 0) {
			score += PawnIsolated;
		}
		
		if ((WhitePassedMask[SQ64(sq)] & pos->pawnsBB[BLACK]) == 0) {
			score += PawnPassed[RanksBrd[sq]];
		}
	}	

	piece = bP;	
	for(int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
		sq = pos->pieceList[piece][pieceNum];
		ASSERT(SqOnBoard(sq));
		score -= PawnTable[MIRROR64(SQ64(sq))];
		
		if ((IsolatedMask[SQ64(sq)] & pos->pawnsBB[BLACK]) == 0) {
			score -= PawnIsolated;
		}
		
		if ((BlackPassedMask[SQ64(sq)] & pos->pawnsBB[WHITE]) == 0) {
			score -= PawnPassed[7 - RanksBrd[sq]];
		}
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

		ASSERT(FileRankValid(FilesBrd[sq]));
		
		if ((pos->pawnsBB[BOTH] & FileBBMask[FilesBrd[sq]]) == 0) {
			score += RookOpenFile;
		} else if ((pos->pawnsBB[WHITE] & FileBBMask[FilesBrd[sq]]) == 0) {
			score += RookSemiOpenFile;
		}
	}	

	piece = bR;	
	for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
		sq = pos->pieceList[piece][pieceNum];
		ASSERT(SqOnBoard(sq));
		score -= RookTable[MIRROR64(SQ64(sq))];

		if ((pos->pawnsBB[BOTH] & FileBBMask[FilesBrd[sq]]) == 0) {
			score -= RookOpenFile;
		} else if ((pos->pawnsBB[BLACK] & FileBBMask[FilesBrd[sq]]) == 0) {
			score -= RookSemiOpenFile;
		}
	}

	piece = wQ;
	for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
		sq = pos->pieceList[piece][pieceNum];
		ASSERT(SqOnBoard(sq));
		ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
		ASSERT(FileRankValid(FilesBrd[sq]));

		if(!(pos->pawnsBB[BOTH] & FileBBMask[FilesBrd[sq]])) {
			score += QueenOpenFile;
		} else if(!(pos->pawnsBB[WHITE] & FileBBMask[FilesBrd[sq]])) {
			score += QueenSemiOpenFile;
		}
	}	

	piece = bQ;	
	for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
		sq = pos->pieceList[piece][pieceNum];
		ASSERT(SqOnBoard(sq));
		ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
		ASSERT(FileRankValid(FilesBrd[sq]));

		if((pos->pawnsBB[BOTH] & FileBBMask[FilesBrd[sq]]) == 0) {
			score -= QueenOpenFile;
		} else if((pos->pawnsBB[BLACK] & FileBBMask[FilesBrd[sq]]) == 0) {
			score -= QueenSemiOpenFile;
		}
	}	

	piece = wK;
	sq = pos->pieceList[piece][0];
	ASSERT(SqOnBoard(sq));
	ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
	
	if ((pos->material[BLACK] <= ENDGAME_MATERIAL)) {
		score += KingE[SQ64(sq)];
	} else {
		score += KingO[SQ64(sq)];
	}
	
	piece = bK;
	sq = pos->pieceList[piece][0];
	ASSERT(SqOnBoard(sq));
	ASSERT(MIRROR64(SQ64(sq)) >= 0 && MIRROR64(SQ64(sq)) <= 63);
	
	if( (pos->material[WHITE] <= ENDGAME_MATERIAL) ) {
		score -= KingE[MIRROR64(SQ64(sq))];
	} else {
		score -= KingO[MIRROR64(SQ64(sq))];
	}
	
	if (pos->pieceNum[wB] >= 2) score += BishopPair;
	if (pos->pieceNum[bB] >= 2) score -= BishopPair;

    // std::cout << "Found a " << piece << " on " << sq << " score: " << score << '\n';
	if (pos->side == WHITE) {
		return score;
	} else {
		return -score;
	}
}