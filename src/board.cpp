#include "board.h"
#include "init.h"
#include "hashkeys.h"
#include "bitboards.h"
#include "data.h"

#include "zobrist.h" // Zobrist hash keys

#include <iostream>
#include <iomanip>

bool CheckBoard(const S_BOARD *pos) {
    int t_pieceNum[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // t_ means temporary
	int t_bigPiece[3] = {0, 0, 0}; //self explanbatory
	int t_majorPiece[3] = {0, 0, 0};
	int t_minorPiece[3] = {0, 0, 0};
	int t_material[3] = {0, 0, 0};

    U64 t_pawnsBB[3] = {0ULL, 0ULL, 0ULL};
    t_pawnsBB[WHITE] = pos->pawnsBB[WHITE];
	t_pawnsBB[BLACK] = pos->pawnsBB[BLACK];
	t_pawnsBB[BOTH] = pos->pawnsBB[BOTH];

    U64 t_knightsBB[3] = {0ULL, 0ULL, 0ULL};
    t_knightsBB[WHITE] = pos->knightsBB[WHITE];
	t_knightsBB[BLACK] = pos->knightsBB[BLACK];
	t_knightsBB[BOTH] = pos->knightsBB[BOTH];

    U64 t_bishopsBB[3] = {0ULL, 0ULL, 0ULL};
    t_bishopsBB[WHITE] = pos->bishopsBB[WHITE];
	t_bishopsBB[BLACK] = pos->bishopsBB[BLACK];
	t_bishopsBB[BOTH] = pos->bishopsBB[BOTH];

    U64 t_rooksBB[3] = {0ULL, 0ULL, 0ULL};
    t_rooksBB[WHITE] = pos->rooksBB[WHITE];
	t_rooksBB[BLACK] = pos->rooksBB[BLACK];
	t_rooksBB[BOTH] = pos->rooksBB[BOTH];

    U64 t_queensBB[3] = {0ULL, 0ULL, 0ULL};
    t_queensBB[WHITE] = pos->queensBB[WHITE];
	t_queensBB[BLACK] = pos->queensBB[BLACK];
	t_queensBB[BOTH] = pos->queensBB[BOTH];

    U64 t_kingsBB[3] = {0ULL, 0ULL, 0ULL};
    t_kingsBB[WHITE] = pos->kingsBB[WHITE];
	t_kingsBB[BLACK] = pos->kingsBB[BLACK];
	t_kingsBB[BOTH] = pos->kingsBB[BOTH];

    // check piece lists
	for (Piece t_piece = wP; t_piece <= bK; ++t_piece) { //looops by piece type from white pawn to black king
		for(int t_piece_num = 0; t_piece_num < pos->pieceNum[t_piece]; ++t_piece_num) { //loops by the poiuece number for each type
			int sq120 = pos->pieceList[t_piece][t_piece_num]; //we get the squarz for that piece
			ASSERT(pos->pieces[sq120] == t_piece); // we assert that if our piece list says a white pawn on this square then our pieces array should have a white pawn on it (they should be the same)
		}                                           //if not, we kjnow that the piece list isnt aligned with what's on the board array
	}

    // check piece count and other counters
	for (int sq64 = 0; sq64 < 64; ++sq64) {
		int sq120 = SQ120(sq64);
		int t_piece = pos->pieces[sq120]; //looking on the piece on that square
		t_pieceNum[t_piece]++; //incrementing the number of temporary pieces for that piece type

		int color = PieceColor[t_piece]; //getting the colour of that piece
		if (PieceIsBig[t_piece] == TRUE) t_bigPiece[color]++; //incrementing depending which typoe of piece it is
		if (PieceIsMinor[t_piece] == TRUE) t_minorPiece[color]++;
		if (PieceIsMajor[t_piece] == TRUE) t_majorPiece[color]++;

		t_material[color] += PieceValue[t_piece]; //then we upodate the material value 
	}
    t_bigPiece[BOTH] = t_bigPiece[WHITE] + t_bigPiece[BLACK];
    t_minorPiece[BOTH] = t_minorPiece[WHITE] + t_minorPiece[BLACK];
    t_majorPiece[BOTH] = t_majorPiece[WHITE] + t_majorPiece[BLACK];
    t_material[BOTH] = t_material[WHITE] + t_material[BLACK];

    for (Piece t_piece = wP; t_piece <= bK; ++t_piece) {
        //     printf("Checking piece: %d, t_pceNum: %d, pos->pceNum: %d\n",
        //    t_piece, t_pceNum[t_piece], pos->pceNum[t_piece]);
            ASSERT(t_pieceNum[t_piece] == pos->pieceNum[t_piece]); //the number of pieces we found on the board and stored inside our temporary piece number array equals for each piece type what our position says
        }                                                   // if they're not equal, something went wrong with the piece number array in our position structure 
    
    // check bitboards count
    //pawns
	int bbcount = CNT(t_pawnsBB[WHITE]); // we take the pawn count for the white bitboard
	ASSERT(bbcount == pos->pieceNum[wP]); // and we assert that count is equal to what the position says is the number of white pawns we have
	bbcount = CNT(t_pawnsBB[BLACK]); //same for black
	ASSERT(bbcount == pos->pieceNum[bP]);
	bbcount = CNT(t_pawnsBB[BOTH]); //same for both
	ASSERT(bbcount == (pos->pieceNum[bP] + pos->pieceNum[wP]));

    //knights
	bbcount = CNT(t_knightsBB[WHITE]);
	ASSERT(bbcount == pos->pieceNum[wN]);
	bbcount = CNT(t_knightsBB[BLACK]);
	ASSERT(bbcount == pos->pieceNum[bN]);
	bbcount = CNT(t_knightsBB[BOTH]);
	ASSERT(bbcount == (pos->pieceNum[bN] + pos->pieceNum[wN]));

    //bishops
	bbcount = CNT(t_bishopsBB[WHITE]);
	ASSERT(bbcount == pos->pieceNum[wB]);
	bbcount = CNT(t_bishopsBB[BLACK]);
	ASSERT(bbcount == pos->pieceNum[bB]);
	bbcount = CNT(t_bishopsBB[BOTH]);
	ASSERT(bbcount == (pos->pieceNum[bB] + pos->pieceNum[wB]));

    //rooks
	bbcount = CNT(t_rooksBB[WHITE]);
	ASSERT(bbcount == pos->pieceNum[wR]);
	bbcount = CNT(t_rooksBB[BLACK]);
	ASSERT(bbcount == pos->pieceNum[bR]);
	bbcount = CNT(t_rooksBB[BOTH]);
	ASSERT(bbcount == (pos->pieceNum[bR] + pos->pieceNum[wR]));

    //queens
	bbcount = CNT(t_queensBB[WHITE]);
	ASSERT(bbcount == pos->pieceNum[wQ]);
	bbcount = CNT(t_queensBB[BLACK]);
	ASSERT(bbcount == pos->pieceNum[bQ]);
	bbcount = CNT(t_queensBB[BOTH]);
	ASSERT(bbcount == (pos->pieceNum[bQ] + pos->pieceNum[wQ]));

    //kings
	bbcount = CNT(t_kingsBB[WHITE]);
	ASSERT(bbcount == pos->pieceNum[wK]);
	bbcount = CNT(t_kingsBB[BLACK]);
	ASSERT(bbcount == pos->pieceNum[bK]);
	bbcount = CNT(t_kingsBB[BOTH]);
	ASSERT(bbcount == (pos->pieceNum[bK] + pos->pieceNum[wK]));

    // check bitboards squares
    //pawns
	while(t_pawnsBB[WHITE]) { //checking if theres a white, black or both pawn oin that square otherwise thre bits don't match with the pieces array in our position and we throw an assert error
		int sq64 = POP(&t_pawnsBB[WHITE]);
		ASSERT(pos->pieces[SQ120(sq64)] == wP); //loop keeps going until there is not bits left
	}

	while(t_pawnsBB[BLACK]) {
		int sq64 = POP(&t_pawnsBB[BLACK]); //same
		ASSERT(pos->pieces[SQ120(sq64)] == bP);
	}

	while(t_pawnsBB[BOTH]) { //same
		int sq64 = POP(&t_pawnsBB[BOTH]);
		ASSERT( (pos->pieces[SQ120(sq64)] == bP) || (pos->pieces[SQ120(sq64)] == wP) );
	}

    //knights
	while(t_knightsBB[WHITE]) {
		int sq64 = POP(&t_knightsBB[WHITE]);
		ASSERT(pos->pieces[SQ120(sq64)] == wN);
	}

	while(t_knightsBB[BLACK]) {
		int sq64 = POP(&t_knightsBB[BLACK]);
		ASSERT(pos->pieces[SQ120(sq64)] == bN);
	}

	while(t_knightsBB[BOTH]) { //same
		int sq64 = POP(&t_knightsBB[BOTH]);
		ASSERT( (pos->pieces[SQ120(sq64)] == bN) || (pos->pieces[SQ120(sq64)] == wN) );
	}

    //bishops
	while(t_bishopsBB[WHITE]) {
		int sq64 = POP(&t_bishopsBB[WHITE]);
		ASSERT(pos->pieces[SQ120(sq64)] == wB);
	}

	while(t_bishopsBB[BLACK]) {
		int sq64 = POP(&t_bishopsBB[BLACK]);
		ASSERT(pos->pieces[SQ120(sq64)] == bB);
	}

	while(t_bishopsBB[BOTH]) { //same
		int sq64 = POP(&t_bishopsBB[BOTH]);
		ASSERT( (pos->pieces[SQ120(sq64)] == bB) || (pos->pieces[SQ120(sq64)] == wB) );
	}

    //rooks
	while(t_rooksBB[WHITE]) {
		int sq64 = POP(&t_rooksBB[WHITE]);
		ASSERT(pos->pieces[SQ120(sq64)] == wR);
	}

	while(t_rooksBB[BLACK]) {
		int sq64 = POP(&t_rooksBB[BLACK]);
		ASSERT(pos->pieces[SQ120(sq64)] == bR);
	}

	while(t_rooksBB[BOTH]) { //same
		int sq64 = POP(&t_rooksBB[BOTH]);
		ASSERT( (pos->pieces[SQ120(sq64)] == bR) || (pos->pieces[SQ120(sq64)] == wR) );
	}

    //queens
	while(t_queensBB[WHITE]) {
		int sq64 = POP(&t_queensBB[WHITE]);
		ASSERT(pos->pieces[SQ120(sq64)] == wQ);
	}

	while(t_queensBB[BLACK]) {
		int sq64 = POP(&t_queensBB[BLACK]);
		ASSERT(pos->pieces[SQ120(sq64)] == bQ);
	}

	while(t_queensBB[BOTH]) { //same
		int sq64 = POP(&t_queensBB[BOTH]);
		ASSERT( (pos->pieces[SQ120(sq64)] == bQ) || (pos->pieces[SQ120(sq64)] == wQ) );
	}

    //kings
	while(t_kingsBB[WHITE]) {
		int sq64 = POP(&t_kingsBB[WHITE]);
		ASSERT(pos->pieces[SQ120(sq64)] == wK);
	}

	while(t_kingsBB[BLACK]) {
		int sq64 = POP(&t_kingsBB[BLACK]);
		ASSERT(pos->pieces[SQ120(sq64)] == bK);
	}

	while(t_kingsBB[BOTH]) { //same
		int sq64 = POP(&t_kingsBB[BOTH]);
		ASSERT( (pos->pieces[SQ120(sq64)] == bK) || (pos->pieces[SQ120(sq64)] == wK) );
	}

    ASSERT(t_material[WHITE] == pos->material[WHITE] && t_material[BLACK] == pos->material[BLACK] && t_material[BOTH] == pos->material[BOTH]);
	ASSERT(t_minorPiece[WHITE] == pos->minorPiece[WHITE] && t_minorPiece[BLACK] == pos->minorPiece[BLACK] && t_minorPiece[BOTH] == pos->minorPiece[BOTH]);
	ASSERT(t_majorPiece[WHITE] == pos->majorPiece[WHITE] && t_majorPiece[BLACK] == pos->majorPiece[BLACK] && t_majorPiece[BOTH] == pos->majorPiece[BOTH]);
	ASSERT(t_bigPiece[WHITE] == pos->bigPiece[WHITE] && t_bigPiece[BLACK] == pos->bigPiece[BLACK] && t_bigPiece[BOTH] == pos->bigPiece[BOTH]);

    ASSERT(pos->side == WHITE || pos->side == BLACK);
    ASSERT(GeneratePosKey(pos) == pos->posKey);//the psition key should be the same as the genreated key
    //when we make a move, we don't at the end of making a move regenerate the entire hashkey becuz thats too many computations
    //so we simply XOR a piece that weve moved into the new square

    ASSERT(pos->enPas == NO_SQ || (RanksBrd[pos->enPas] == RANK_6 && pos->side == WHITE) //check for en passant (its either no square or it must be a square that if the side to move is white
            // is on the 6th rank if the side to move is black then the en passant square must be on the 3rd rank
        || (RanksBrd[pos->enPas] == RANK_3 && pos->side == BLACK));

    ASSERT(pos->pieces[pos->kingSquare[WHITE]] == wK); //checking the square of kings if they are the corresponding ones  
	ASSERT(pos->pieces[pos->kingSquare[BLACK]] == bK);

	ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);

	return TRUE; //assuming we passed everything, we return true 
}

void UpdateListsMaterial(S_BOARD *pos) {
    for (int i = 0; i < BRD_SQ_NUM; ++i) {
        int sq = i;
        int piece = pos->pieces[i];

        if (piece != OFFBOARD && piece != EMPTY) {
            int color = PieceColor[piece];
            if (PieceIsBig[piece] == TRUE) pos->bigPiece[color]++;
            if (PieceIsMajor[piece] == TRUE) pos->majorPiece[color]++;
            if (PieceIsMinor[piece] == TRUE) pos->minorPiece[color]++;
            
            pos->material[color] += PieceValue[piece];

            //for example : pList[wP][0] = a1;
            // pList[wP][1] = a2;
            ASSERT(pos->pieceNum[piece] >= 0 && pos->pieceNum[piece] < 10);
            if (piece == wP || piece == bP) {
                ASSERT(pos->pieceNum[piece] >= 0 && pos->pieceNum[piece] < 8);
            }
            if (piece == wK || piece == bK) {
                ASSERT(pos->pieceNum[piece] >= 0 && pos->pieceNum[piece] < 1);
            }
            pos->pieceList[piece][pos->pieceNum[piece]] = sq;
            pos->pieceNum[piece]++;

            if(piece == wK) pos->kingSquare[WHITE] = sq;
			if(piece == bK) pos->kingSquare[BLACK] = sq;

            //set pieces bitboard
            //pawns bitboard
            if (piece == wP) { //if the piece is a white pawn, set the bit on the bitboard in a position for white pawn and both
				SETBIT(&(pos->pawnsBB[WHITE]), SQ64(sq)); //we coinvert it cuz sq is in a 120 format 
				SETBIT(&(pos->pawnsBB[BOTH]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[WHITE]), SQ64(sq));
				SETBIT(&(pos->allPiecesBB[BOTH]), SQ64(sq));
			} else if(piece == bP) { //same for black pawn
				SETBIT(&(pos->pawnsBB[BLACK]), SQ64(sq)); 
				SETBIT(&(pos->pawnsBB[BOTH]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[BLACK]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[BOTH]), SQ64(sq));
			}

            //knights bitboard
            if (piece == wN) {
				SETBIT(&(pos->knightsBB[WHITE]), SQ64(sq));
				SETBIT(&(pos->knightsBB[BOTH]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[WHITE]), SQ64(sq));
				SETBIT(&(pos->allPiecesBB[BOTH]), SQ64(sq));
			} else if(piece == bN) {
				SETBIT(&(pos->knightsBB[BLACK]), SQ64(sq));
				SETBIT(&(pos->knightsBB[BOTH]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[BLACK]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[BOTH]), SQ64(sq));
			}

            //bishops bitboard
            if (piece == wB) {
				SETBIT(&(pos->bishopsBB[WHITE]), SQ64(sq));
				SETBIT(&(pos->bishopsBB[BOTH]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[WHITE]), SQ64(sq));
				SETBIT(&(pos->allPiecesBB[BOTH]), SQ64(sq));
			} else if(piece == bB) {
				SETBIT(&(pos->bishopsBB[BLACK]), SQ64(sq));
				SETBIT(&(pos->bishopsBB[BOTH]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[BLACK]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[BOTH]), SQ64(sq));
			}

            //rooks bitboard
            if (piece == wR) {
				SETBIT(&(pos->rooksBB[WHITE]), SQ64(sq));
				SETBIT(&(pos->rooksBB[BOTH]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[WHITE]), SQ64(sq));
				SETBIT(&(pos->allPiecesBB[BOTH]), SQ64(sq));
			} else if(piece == bR) {
				SETBIT(&(pos->rooksBB[BLACK]), SQ64(sq));
				SETBIT(&(pos->rooksBB[BOTH]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[BLACK]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[BOTH]), SQ64(sq));
			}

            //queens bitboard
            if (piece == wQ) {
				SETBIT(&(pos->queensBB[WHITE]), SQ64(sq));
				SETBIT(&(pos->queensBB[BOTH]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[WHITE]), SQ64(sq));
				SETBIT(&(pos->allPiecesBB[BOTH]), SQ64(sq));
			} else if(piece == bQ) {
				SETBIT(&(pos->queensBB[BLACK]), SQ64(sq));
				SETBIT(&(pos->queensBB[BOTH]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[BLACK]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[BOTH]), SQ64(sq));
			}

            //kings bitboard
            if (piece == wK) {
				SETBIT(&(pos->kingsBB[WHITE]), SQ64(sq));
				SETBIT(&(pos->kingsBB[BOTH]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[WHITE]), SQ64(sq));
				SETBIT(&(pos->allPiecesBB[BOTH]), SQ64(sq));
			} else if(piece == bK) {
				SETBIT(&(pos->kingsBB[BLACK]), SQ64(sq));
				SETBIT(&(pos->kingsBB[BOTH]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[BLACK]), SQ64(sq));
                SETBIT(&(pos->allPiecesBB[BOTH]), SQ64(sq));
			}
        }
    }

    pos->bigPiece[BOTH] = pos->bigPiece[WHITE] + pos->bigPiece[BLACK];
    pos->majorPiece[BOTH] = pos->majorPiece[WHITE] + pos->majorPiece[BLACK];
    pos->minorPiece[BOTH] = pos->minorPiece[WHITE] + pos->minorPiece[BLACK];
    pos->material[BOTH] = pos->material[WHITE] + pos->material[BLACK];
}

int ParseFen(char *fen, S_BOARD *pos) {
    ASSERT(fen != NULL);
    ASSERT(pos != NULL);

    Rank rank = RANK_8;
    File file = FILE_A;

	ResetBoard(pos);

    while ((rank >= RANK_1) && *fen) {
        int piece = 0;
        int count = 1;

        switch (*fen) {
            case 'p': piece = bP; break;
            case 'r': piece = bR; break;
            case 'n': piece = bN; break;
            case 'b': piece = bB; break;
            case 'k': piece = bK; break;
            case 'q': piece = bQ; break;
            case 'P': piece = wP; break;
            case 'R': piece = wR; break;
            case 'N': piece = wN; break;
            case 'B': piece = wB; break;
            case 'K': piece = wK; break;
            case 'Q': piece = wQ; break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                count = *fen - '0';
                break;

            case '/':
            case ' ':
                --rank;
                file = FILE_A;
                fen++;
                continue;

            default:
                std::cerr << "FEN error \n";
                return -1;
        }
        
        for (int i = 0; i < count; ++i) {
            int sq64 = rank * 8 + file;
			int sq120 = SQ120(sq64);
            if (piece != EMPTY) {
                pos->pieces[sq120] = piece;
            }
			++file;
        }
		fen++;
    }

    ASSERT(*fen == 'w' || *fen == 'b');
    pos->side = (*fen == 'w') ? WHITE : BLACK;
	fen += 2; //move to enPas

    for (int i = 0; i < 4; ++i) {
        if (*fen == ' ') {
            break;
        }
        switch (*fen) {
            case 'K': pos->castlePerm |= WKCA; break;
            case 'Q': pos->castlePerm |= WQCA; break;
            case 'k': pos->castlePerm |= BKCA; break;
            case 'q': pos->castlePerm |= BQCA; break;
            default:    break;
        }
        fen++;
    }
    fen++;

    ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);

    if (*fen != '-') {
        file = File(fen[0] - 'a');
        rank = Rank(fen[1] - '1');
        ASSERT(file >= FILE_A && file <= FILE_H);
        ASSERT(rank >= RANK_1 && rank <= RANK_8);
        pos->enPas = FR2SQ(file, rank);
    }

    pos->posKey = GeneratePosKey(pos);
    pos->pawnKey = calculate_initial_pawn_key(pos);// pawn key is the same as position key but only for pawns

    UpdateListsMaterial(pos);

    return 0;
}

void ResetBoard(S_BOARD *pos) {
    for (int i = 0; i < BRD_SQ_NUM; ++i){ //looping through all 120 squares
        pos->pieces[i] = OFFBOARD; // and setting them to "OFFBOARD"
    } //for more understanding, look at the image in the folder named "board representation"

    for (int i = 0; i < SQUARE_NB; ++i) {
        pos->pieces[SQ120(i)] = EMPTY; // then the "real" squares are set to "empty"
    }

    for (int i = 0; i < 3; ++i) {
        pos->bigPiece[i] = 0; // number of all pieces (black and white)
        pos->majorPiece[i] = 0;
        pos->minorPiece[i] = 0;
        pos->material[i] = 0; //set material scores to 0
    }

    for (int i = 0; i < 3; i++) {
        pos->pawnsBB[i] = 0ULL; // OULL cuz pawnsBB are an U64 type in an unsigned long long
        pos->knightsBB[i] = 0ULL;
        pos->bishopsBB[i] = 0ULL;
        pos->rooksBB[i] = 0ULL;
        pos->queensBB[i] = 0ULL;
        pos->kingsBB[i] = 0ULL;
        pos->allPiecesBB[i] = 0ULL; //all pieces bitboard
    }

    for (int i = 0; i < 13; ++i) {
        pos->pieceNum[i] = 0; //resets the piece number
    }

    pos->kingSquare[WHITE] = pos->kingSquare[BLACK] = NO_SQ;

    pos->side = BOTH; //once we setup a position, checkboard will check some things so we set it to both not to get an ASSERT
    pos->enPas = NO_SQ; 
    pos->fiftyMove = 0;

    pos->ply = 0; //nber of half moves played in the current search
    pos->hisPly = 0;

    pos->castlePerm = 0;

    pos->posKey = 0ULL;
    pos->pawnKey = 0ULL; // pawn key is the same as position key but only for pawns
}

void PrintBoard(const S_BOARD *pos) {

	std::cout << "\nGame Board:\n\n"; 

	for (Rank rank = RANK_8; rank >= RANK_1; --rank) { // stat on the 8th rank cuz its how we look at it
		printf("%d  ",rank+1);
		for (File file = FILE_A; file <= FILE_H; ++file) {
			int sq = FR2SQ(file,rank); //sqare index
			int piece = pos->pieces[sq]; // get the piece using the square index from board
            std::cout << std::setw(3) << PieceChar[piece];
		}
        std::cout << '\n';
	}

    std::cout << "\n   ";
    for (File file = FILE_A; file <= FILE_H; ++file) {
        std::cout << std::setw(3) << char('a' + file);
    }
    std::cout << '\n';

    std::cout << "side: " << SideChar[pos->side] << '\n';
    std::cout << "enPas: " << std::dec << pos->enPas << '\n'; //printed as a decimal rather then as characters
    std::cout << "castle: "
		    << (pos->castlePerm & WKCA ? 'K' : '-') //if castle perm ends with ' ' then print the corresponding letters
			<< (pos->castlePerm & WQCA ? 'Q' : '-')
			<< (pos->castlePerm & BKCA ? 'k' : '-')
			<< (pos->castlePerm & BQCA ? 'q' : '-')
			<< '\n';
    std::cout << "PosKey: ";
    std::cout << std::hex << std::uppercase << pos->posKey << '\n' << std::dec; //hexa cuz easier to read
    std::cout << std::hex << std::uppercase << pos->pawnKey << '\n' << std::dec; //hexa cuz easier to read
}

void MirrorBoard(S_BOARD *pos) {
    int tempPiecesArray[64];
    int tempSide = pos->side^1;
	int SwapPiece[13] = { EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK };
    int tempCastlePerm = 0;
    int tempEnPas = NO_SQ;

	int sq;
	int tp;

    if (pos->castlePerm & WKCA) tempCastlePerm |= BKCA;
    if (pos->castlePerm & WQCA) tempCastlePerm |= BQCA;

    if (pos->castlePerm & BKCA) tempCastlePerm |= WKCA;
    if (pos->castlePerm & BQCA) tempCastlePerm |= WQCA;

	if (pos->enPas != NO_SQ)  {
        tempEnPas = SQ120(Mirror64[SQ64(pos->enPas)]);
    }

    for (sq = 0; sq < 64; sq++) {
        tempPiecesArray[sq] = pos->pieces[SQ120(Mirror64[sq])];
    }

    ResetBoard(pos);

	for (sq = 0; sq < 64; sq++) {
        tp = SwapPiece[tempPiecesArray[sq]];
        pos->pieces[SQ120(sq)] = tp;
    }

	pos->side = tempSide;
    pos->castlePerm = tempCastlePerm;
    pos->enPas = tempEnPas;

    pos->posKey = GeneratePosKey(pos);
    pos->pawnKey = calculate_initial_pawn_key(pos); // <<< THÊM DÒNG NÀY: Tính lại pawnKey

	UpdateListsMaterial(pos);

    ASSERT(CheckBoard(pos));
}


SF::Key calculate_initial_pawn_key(const S_BOARD* pos) {
    SF::Key finalKey = 0;
    // Đảm bảo Zobrist::init() đã được gọi trước đó!
    for (SF::Color c : { SF::WHITE, SF::BLACK }) {
        SF::Bitboard pawns = pos->pawnsBB[c];
        // Lấy enum Piece tương ứng từ types.h
        int vicePawnEnum = (c == SF::WHITE) ? wP : bP;
        while (pawns) {
            SF::Square s = SF::pop_lsb(&pawns);
            // Giả định Zobrist::Psq[vicePawnEnum][s] đã được khởi tạo
            finalKey ^= Zobrist::Psq[vicePawnEnum][s];
        }
    }
    return finalKey;
}