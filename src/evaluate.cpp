#include "evaluate.h"
#include "validate.h"
#include "init.h"
#include "data.h"
#include "io.h"
#include "pawn_analysis.h"
#include "king_safety.h"



namespace {
    // Trọng số chuẩn (ví dụ: Stockfish)
    constexpr int KnightPhase = 1;
    constexpr int BishopPhase = 1;
    constexpr int RookPhase   = 2;
    constexpr int QueenPhase  = 4;
    constexpr int TotalPhase = (KnightPhase * 4) + (BishopPhase * 4) + (RookPhase * 4) + (QueenPhase * 2); // = 24

    int calculate_game_phase(const S_BOARD* pos) {
        int phase = TotalPhase; // Bắt đầu với phase tối đa
        // Trừ đi phase cho mỗi quân bị mất
        phase -= (pos->pieceNum[wN] + pos->pieceNum[bN]) * KnightPhase;
        phase -= (pos->pieceNum[wB] + pos->pieceNum[bB]) * BishopPhase;
        phase -= (pos->pieceNum[wR] + pos->pieceNum[bR]) * RookPhase;
        phase -= (pos->pieceNum[wQ] + pos->pieceNum[bQ]) * QueenPhase;

        // Scale và giới hạn phase trong khoảng [0, 256]
        // Đảm bảo TotalPhase không bao giờ bằng 0 để tránh chia cho 0
        // (Luôn còn vua nên TotalPhase > 0)
        // Nhân với 256 trước để tránh mất độ chính xác
         int scaledPhase = (phase * 256 + (TotalPhase / 2)) / TotalPhase; // Thêm (TotalPhase / 2) để làm tròn
         // return std::min(scaledPhase, 256); // Không cần min vì scale đã nằm trong khoảng

         // Đảm bảo không nhỏ hơn 0 (do lỗi làm tròn có thể xảy ra)
         return std::max(0, scaledPhase);
    }
}



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
    
	int endGameMaterial = ENDGAME_MATERIAL;

	if (!pos->pieceNum[wP] && !pos->pieceNum[bP] && MaterialDraw(pos) == TRUE) {
		return 0;
	}

    // piece = wP;	
	// for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
	// 	sq = pos->pieceList[piece][pieceNum];
	// 	ASSERT(SqOnBoard(sq));
	// 	score += PawnTable[SQ64(sq)];

	// 	if ((IsolatedMask[SQ64(sq)] & pos->pawnsBB[WHITE]) == 0) {
	// 		score += PawnIsolated;
	// 	}
		
	// 	if ((WhitePassedMask[SQ64(sq)] & pos->pawnsBB[BLACK]) == 0) {
	// 		score += PawnPassed[RanksBrd[sq]];
	// 	}
	// }	

	// piece = bP;	
	// for(int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
	// 	sq = pos->pieceList[piece][pieceNum];
	// 	ASSERT(SqOnBoard(sq));
	// 	score -= PawnTable[MIRROR64(SQ64(sq))];
		
	// 	if ((IsolatedMask[SQ64(sq)] & pos->pawnsBB[BLACK]) == 0) {
	// 		score -= PawnIsolated;
	// 	}
		
	// 	if ((BlackPassedMask[SQ64(sq)] & pos->pawnsBB[WHITE]) == 0) {
	// 		score -= PawnPassed[7 - RanksBrd[sq]];
	// 	}
	// }

	const PawnEntry* pawnEntry = probe_pawn_table(pos);
	if (pawnEntry) {
        int mg = eval_help::mg_value(pawnEntry->scores[WHITE]) - eval_help::mg_value(pawnEntry->scores[BLACK]);
		int eg = eval_help::eg_value(pawnEntry->scores[WHITE]) - eval_help::eg_value(pawnEntry->scores[BLACK]);

		int phase = calculate_game_phase(pos); // 0–256
		int blended = ((mg * phase) + (eg * (256 - phase))) >> 8;
		score += blended;
		

		if (phase >= 185) {
			int king_mg = eval_help::mg_value(CalculateSimpleKingSafetyScore(pos, WHITE)) - eval_help::mg_value(CalculateSimpleKingSafetyScore(pos, BLACK));
			int king_eg = eval_help::eg_value(CalculateSimpleKingSafetyScore(pos, WHITE)) - eval_help::eg_value(CalculateSimpleKingSafetyScore(pos, BLACK));
			int king_safety = ((king_mg * phase) + (king_eg * (256 - phase))) >> 8;
			score += king_safety;
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
	
	if ((pos->material[BLACK] <= endGameMaterial)) {
		score += KingE[SQ64(sq)];
	} else {
		score += KingO[SQ64(sq)];
	}
	
	piece = bK;
	sq = pos->pieceList[piece][0];
	ASSERT(SqOnBoard(sq));
	ASSERT(MIRROR64(SQ64(sq)) >= 0 && MIRROR64(SQ64(sq)) <= 63);
	
	if( (pos->material[WHITE] <= endGameMaterial) ) {
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