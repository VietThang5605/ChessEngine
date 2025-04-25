#include "movegen.h"

void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

void AddCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

void AddEnPassantMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

void AddWhitePawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {
	ASSERT(PieceValidEmpty(cap));
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	ASSERT(CheckBoard(pos));

	if (RanksBrd[from] == RANK_7) {
		AddCaptureMove(pos, MOVE(from,to,cap,wQ,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,wR,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,wB,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,wN,0), list);
	} else {
		AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
	}
}

void AddWhitePawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	ASSERT(CheckBoard(pos));

	if(RanksBrd[from] == RANK_7) {
		AddQuietMove(pos, MOVE(from,to,EMPTY,wQ,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,wR,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,wB,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,wN,0), list);
	} else {
		AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
	}
}

void AddBlackPawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {
	ASSERT(PieceValidEmpty(cap));
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	ASSERT(CheckBoard(pos));

	if (RanksBrd[from] == RANK_2) {
		AddCaptureMove(pos, MOVE(from,to,cap,bQ,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,bR,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,bB,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,bN,0), list);
	} else {
		AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
	}
}

void AddBlackPawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	ASSERT(CheckBoard(pos));

	if(RanksBrd[from] == RANK_2) {
		AddQuietMove(pos, MOVE(from,to,EMPTY,bQ,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,bR,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,bB,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,bN,0), list);
	} else {
		AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
	}
}

void GenerateAllMove(const S_BOARD *pos, S_MOVELIST *list) {
    ASSERT(CheckBoard(pos));

	list->count = 0;

	int side = pos->side;

    std::cout << "\nSide: " << side << '\n';

	if (side == WHITE) {
		for (int pieceNum = 0; pieceNum < pos->pieceNum[wP]; ++pieceNum) {
			int sq = pos->pieceList[wP][pieceNum];
			ASSERT(SqOnBoard(sq));

			if (pos->pieces[sq + 10] == EMPTY) {
				AddWhitePawnMove(pos, sq, sq+10, list);
				if(RanksBrd[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY) {
					AddQuietMove(pos, MOVE(sq,(sq+20),EMPTY,EMPTY,MOVEFLAG_PAWNSTART),list);
				}
			}

			if (!SQOFFBOARD(sq + 9) && PieceColor[pos->pieces[sq + 9]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, sq+9, pos->pieces[sq + 9], list);
			}
			if (!SQOFFBOARD(sq + 11) && PieceColor[pos->pieces[sq + 11]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, sq+11, pos->pieces[sq + 11], list);
			}

			if (pos->enPas != NO_SQ) {
				if(sq + 9 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq + 9,EMPTY,EMPTY,MOVEFLAG_EP), list);
				}
				if(sq + 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq + 11,EMPTY,EMPTY,MOVEFLAG_EP), list);
				}
			}

            // if (pos->enPas != NO_SQ) {
			// 	if (sq + 9 == pos->enPas) {
			// 		AddCaptureMove(pos, MOVE(sq,sq + 9,EMPTY,EMPTY,MOVEFLAG_EP), list);
			// 	}
			// 	if (sq + 11 == pos->enPas) {
			// 		AddCaptureMove(pos, MOVE(sq,sq + 11,EMPTY,EMPTY,MOVEFLAG_EP), list);
			// 	}
			// }
		}

		// castling
		if (pos->castlePerm & WKCA) {
			if (pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
				if(!SqAttacked(E1,BLACK,pos) && !SqAttacked(F1,BLACK,pos) ) {
					AddQuietMove(pos, MOVE(E1, G1, EMPTY, EMPTY, MOVEFLAG_CASTLE), list);
				}
			}
		}

		if (pos->castlePerm & WQCA) {
			if(pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY) {
				if(!SqAttacked(E1,BLACK,pos) && !SqAttacked(D1,BLACK,pos) ) {
					AddQuietMove(pos, MOVE(E1, C1, EMPTY, EMPTY, MOVEFLAG_CASTLE), list);
				}
			}
		}

	} else {
        for (int pieceNum = 0; pieceNum < pos->pieceNum[bP]; ++pieceNum) {
			int sq = pos->pieceList[bP][pieceNum];
			ASSERT(SqOnBoard(sq));
            
            if (pos->pieces[sq - 10] == EMPTY) {
				AddBlackPawnMove(pos, sq, sq-10, list);
				if(RanksBrd[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY) {
					AddQuietMove(pos, MOVE(sq,(sq-20),EMPTY,EMPTY,MOVEFLAG_PAWNSTART),list);
				}
			}

            if (!SQOFFBOARD(sq - 9) && PieceColor[pos->pieces[sq - 9]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq-9, pos->pieces[sq - 9], list);
			}
			if (!SQOFFBOARD(sq - 11) && PieceColor[pos->pieces[sq - 11]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, sq-11, pos->pieces[sq - 11], list);
			}

            if (pos->enPas != NO_SQ) {
				if(sq - 9 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq - 9,EMPTY,EMPTY,MOVEFLAG_EP), list);
				}
				if(sq - 11 == pos->enPas) {
					AddEnPassantMove(pos, MOVE(sq,sq - 11,EMPTY,EMPTY,MOVEFLAG_EP), list);
				}
			}
        }

		// castling
		if (pos->castlePerm & BKCA) {
			if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
				if(!SqAttacked(E8,WHITE,pos) && !SqAttacked(F8,WHITE,pos) ) {
					AddQuietMove(pos, MOVE(E8, G8, EMPTY, EMPTY, MOVEFLAG_CASTLE), list);
				}
			}
		}

		if (pos->castlePerm &  BQCA) {
			if(pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY) {
				if(!SqAttacked(E8,WHITE,pos) && !SqAttacked(D8,WHITE,pos) ) {
					AddQuietMove(pos, MOVE(E8, C8, EMPTY, EMPTY, MOVEFLAG_CASTLE), list);
				}
			}
		}
    }

    /* Loop for slide pieces */
	int pieceIndex = LoopSlideIndex[side];
	int piece = LoopSlidePiece[pieceIndex++];
    while (piece != 0) {
        ASSERT(PieceValid(piece));
        std::cout << "sliders pieceIndex: " << pieceIndex << " piece: " << piece << '\n';

        for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
			int sq = pos->pieceList[piece][pieceNum];
			ASSERT(SqOnBoard(sq));
            std::cout << "Piece: " << PieceChar[piece] << " on " << PrintSquare(sq) << '\n';

            for (int i = 0; i < NumDir[piece]; i++) {
                int dir = PieceDir[piece][i]; //dir can't be 0 because we have i < NumDir[piece]
				int t_sq = sq + dir;

                while (!SQOFFBOARD(t_sq)) {
                    // BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
                    if (pos->pieces[t_sq] != EMPTY) {
                        if (PieceColor[pos->pieces[t_sq]] == (side ^ 1)) {
                            AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                        }
                        break;
                    }
                    AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
                    t_sq += dir;
                }
            }
        }

		piece = LoopSlidePiece[pieceIndex++];
	}

    /* Loop for non slide */
    pieceIndex = LoopNonSlideIndex[side];
	piece = LoopNonSlidePiece[pieceIndex++];
    while (piece != 0) {
        ASSERT(PieceValid(piece));
        std::cout << "non-sliders pieceIndex: " << pieceIndex << " piece: " << piece << '\n';

        for (int pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
			int sq = pos->pieceList[piece][pieceNum];
			ASSERT(SqOnBoard(sq));
            std::cout << "Piece: " << PieceChar[piece] << " on " << PrintSquare(sq) << '\n';

            for (int i = 0; i < NumDir[piece]; i++) {
                int dir = PieceDir[piece][i];
				int t_sq = sq + dir;

                if (SQOFFBOARD(t_sq)) {
				    continue;
				}

                // BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
				if (pos->pieces[t_sq] != EMPTY) {
					if (PieceColor[pos->pieces[t_sq]] == (side ^ 1)) {
						AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
					}
					continue;
				}
				AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
            }
        }

		piece = LoopNonSlidePiece[pieceIndex++];
	}
}