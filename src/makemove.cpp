#include "makemove.h"

static void ClearPiece(const int sq, S_BOARD *pos) {
    ASSERT(SqOnBoard(sq)); // see if the square's on the board to acvoid problems
	ASSERT(CheckBoard(pos));
	
    int piece = pos->pieces[sq]; // and get the piece that's on that square

    ASSERT(PieceValid(piece)); // and double check if it's a piece (just in case)
    
    int color = PieceColor[piece];  //color of the piece
	
	ASSERT(SideValid(color));
	
    HASH_PIECE(piece,sq); // XOR our pieces on that square
	
	pos->pieces[sq] = EMPTY; // set thatsquare to empty
    pos->material[color] -= PieceValue[piece]; //substract the value from the material score
    pos->material[BOTH] -= PieceValue[piece];
	
    if (PieceIsBig[piece]) { //straight forward
        pos->bigPiece[color]--;
        pos->bigPiece[BOTH]--;

        if (PieceIsMajor[piece]) {
            pos->majorPiece[color]--;
            pos->majorPiece[BOTH]--;
        } else {
            pos->minorPiece[color]--;
            pos->minorPiece[BOTH]--;
        }

        if (IsKnight(piece)) {
            CLRBIT(&pos->knightsBB[color],SQ64(sq));
            CLRBIT(&pos->knightsBB[BOTH],SQ64(sq));
        } else if (IsBishop(piece)) {
            CLRBIT(&pos->bishopsBB[color],SQ64(sq));
            CLRBIT(&pos->bishopsBB[BOTH],SQ64(sq));
        } else if (IsRook(piece)) {
            CLRBIT(&pos->rooksBB[color],SQ64(sq));
            CLRBIT(&pos->rooksBB[BOTH],SQ64(sq));
        } else if (IsQueen(piece)) {
            CLRBIT(&pos->queensBB[color],SQ64(sq));
            CLRBIT(&pos->queensBB[BOTH],SQ64(sq));
        } else {
            CLRBIT(&pos->kingsBB[color],SQ64(sq));
            CLRBIT(&pos->kingsBB[BOTH],SQ64(sq));
        }
        
    } else { // need to clear the bitboard for the pawns of the color of this piece
        CLRBIT(&pos->pawnsBB[color],SQ64(sq));
        CLRBIT(&pos->pawnsBB[BOTH],SQ64(sq));
    }
    CLRBIT(&pos->allPiecesBB[color],SQ64(sq));
    CLRBIT(&pos->allPiecesBB[BOTH],SQ64(sq));

    /*
        for the next part: 
            pos->pceNum[wP] == 5 looping from 0 to 4
            pos->pList[pce][0] == sq0
            pos->pList[pce][1] == sq1
            pos->pList[pce][2] == sq2
            pos->pList[pce][3] == sq3
            pos->pList[pce][3] == sq4

        so one of these square should be equal to the square that we put on argument
        lets say it's sq3 so t_pceNum =3
    */
    int t_pieceNum = -1; // will be xplained later down in the piece lsit
	for (int i = 0; i < pos->pieceNum[piece]; ++i) {
		if (pos->pieceList[piece][i] == sq) {
			t_pieceNum = i;
			break;
		}
	}
	
	ASSERT(t_pieceNum != -1); //another check which isn't necessary but just in case the checkboard function didnt do its job
	ASSERT(t_pieceNum >= 0 && t_pieceNum < 10);
	
	pos->pieceNum[piece]--;		
    // so now, we have pos->pieceNum[wP] == 4
	
	pos->pieceList[piece][t_pieceNum] = pos->pieceList[piece][pos->pieceNum[piece]];
    // pos->pList[pce][3] = pos->pList[wP][4] = sq4
    // so the square3 value has been deleted and that's how we remove a piece from the baord
}

static void AddPiece(const int sq, S_BOARD *pos, const int piece) {
    ASSERT(PieceValid(piece));
    ASSERT(SqOnBoard(sq));
	
	int color = PieceColor[piece];
	ASSERT(SideValid(color));

    HASH_PIECE(piece,sq);

    pos->pieces[sq] = piece;

    if (PieceIsBig[piece]) {
        pos->bigPiece[color]++;
        pos->bigPiece[BOTH]++;

		if(PieceIsMajor[piece]) {
			pos->majorPiece[color]++;
			pos->majorPiece[BOTH]++;
		} else {
			pos->minorPiece[color]++;
			pos->minorPiece[BOTH]++;
		}

        if (IsKnight(piece)) {
            SETBIT(&pos->knightsBB[color],SQ64(sq));
            SETBIT(&pos->knightsBB[BOTH],SQ64(sq));
        } else if (IsBishop(piece)) {
            SETBIT(&pos->bishopsBB[color],SQ64(sq));
            SETBIT(&pos->bishopsBB[BOTH],SQ64(sq));
        } else if (IsRook(piece)) {
            SETBIT(&pos->rooksBB[color],SQ64(sq));
            SETBIT(&pos->rooksBB[BOTH],SQ64(sq));
        } else if (IsQueen(piece)) {
            SETBIT(&pos->queensBB[color],SQ64(sq));
            SETBIT(&pos->queensBB[BOTH],SQ64(sq));
        } else {
            SETBIT(&pos->kingsBB[color],SQ64(sq));
            SETBIT(&pos->kingsBB[BOTH],SQ64(sq));
        }

	} else {
		SETBIT(&pos->pawnsBB[color],SQ64(sq));
		SETBIT(&pos->pawnsBB[BOTH],SQ64(sq));
	}
	
	pos->material[color] += PieceValue[piece];
	pos->material[BOTH] += PieceValue[piece];
	pos->pieceList[piece][pos->pieceNum[piece]++] = sq;
}

static void MovePiece(const int from, const int to, S_BOARD *pos) {

    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
	
	int piece = pos->pieces[from];
	int color = PieceColor[piece];

	ASSERT(SideValid(color));
    ASSERT(PieceValid(piece));
	
#ifdef DEBUG
	int t_PieceNum = FALSE; // make sure that we find what we're looking for
#endif

	HASH_PIECE(piece,from);
	pos->pieces[from] = EMPTY;
	
	HASH_PIECE(piece,to);
	pos->pieces[to] = piece;
	
	if (!PieceIsBig[piece]) {
		CLRBIT(&pos->pawnsBB[color],SQ64(from));
		CLRBIT(&pos->pawnsBB[BOTH],SQ64(from));
		SETBIT(&pos->pawnsBB[color],SQ64(to));
		SETBIT(&pos->pawnsBB[BOTH],SQ64(to));
	} else {
        if (IsKnight(piece)) {
            CLRBIT(&pos->knightsBB[color],SQ64(from));
            CLRBIT(&pos->knightsBB[BOTH],SQ64(from));
            SETBIT(&pos->knightsBB[color],SQ64(to));
            SETBIT(&pos->knightsBB[BOTH],SQ64(to));
        } else if (IsBishop(piece)) {
            CLRBIT(&pos->bishopsBB[color],SQ64(from));
            CLRBIT(&pos->bishopsBB[BOTH],SQ64(from));
            SETBIT(&pos->bishopsBB[color],SQ64(to));
            SETBIT(&pos->bishopsBB[BOTH],SQ64(to));
        } else if (IsRook(piece)) {
            CLRBIT(&pos->rooksBB[color],SQ64(from));
            CLRBIT(&pos->rooksBB[BOTH],SQ64(from));
            SETBIT(&pos->rooksBB[color],SQ64(to));
            SETBIT(&pos->rooksBB[BOTH],SQ64(to));
        } else if (IsQueen(piece)) {
            CLRBIT(&pos->queensBB[color],SQ64(from));
            CLRBIT(&pos->queensBB[BOTH],SQ64(from));
            SETBIT(&pos->queensBB[color],SQ64(to));
            SETBIT(&pos->queensBB[BOTH],SQ64(to));
        } else {
            CLRBIT(&pos->kingsBB[color],SQ64(from));
            CLRBIT(&pos->kingsBB[BOTH],SQ64(from));
            SETBIT(&pos->kingsBB[color],SQ64(to));
            SETBIT(&pos->kingsBB[BOTH],SQ64(to));
        }
    }
	
	for (int i = 0; i < pos->pieceNum[piece]; ++i) {
		if (pos->pieceList[piece][i] == from) {
			pos->pieceList[piece][i] = to;
#ifdef DEBUG
			t_PieceNum = TRUE;
#endif
			break;
		}
	}
	ASSERT(t_PieceNum);
}