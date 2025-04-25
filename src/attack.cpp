#include "attack.h"

bool SqAttacked(const int sq, const int side, const S_BOARD *pos) {
    //what square, which side is attacking it and in what position basically
    
	ASSERT(SqOnBoard(sq));
	ASSERT(SideValid(side));
	ASSERT(CheckBoard(pos));

    // pawns
	if (side == WHITE) {
		if (pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) {
			return TRUE;
		}
	} else {
		if (pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP) {
			return TRUE;
        }
	}

    // knights
	for (int i = 0; i < 8; ++i) {		
		int piece = pos->pieces[sq + KnightDir[i]];
		// ASSERT(PceValidEmptyOffbrd(pce));
        //EMPTY is also in PieceIsKinght and it's value is FALSE
		if (piece != OFFBOARD && IsKnight(piece) && PieceColor[piece] == side) {
			return TRUE;
		}
	}

    // rooks, queens
	for (int i = 0; i < 4; ++i) {		
		int dir = RookDir[i];
		int t_sq = sq + dir;
		int piece = pos->pieces[t_sq];
		while (piece != OFFBOARD) {
			if (piece != EMPTY) {
				if (IsRookQueen(piece) && PieceColor[piece] == side) {
					return TRUE;
				}
				break;
			}
			t_sq += dir;
			piece = pos->pieces[t_sq];
		}
	}

    // bishops, queens
	for (int i = 0; i < 4; ++i) {		
		int dir = BishopDir[i];
		int t_sq = sq + dir;
		int piece = pos->pieces[t_sq];
		while (piece != OFFBOARD) {
			if (piece != EMPTY) {
				if (IsBishopQueen(piece) && PieceColor[piece] == side) {
					return TRUE;
				}
				break;
			}
			t_sq += dir;
			piece = pos->pieces[t_sq];
		}
	}

    // kings
	for (int i = 0; i < 8; ++i) {		
		int piece = pos->pieces[sq + KingDir[i]];
		if (piece != OFFBOARD && IsKing(piece) && PieceColor[piece] == side) {
			return TRUE;
		}
	}

    return FALSE;
}