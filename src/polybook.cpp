#include "polybook.h"
#include "polykeys.h"
#include "init.h"

const int PolyKindOfPiece[13] = {
	-1, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10
};

long long NumEntries = 0;

S_POLY_BOOK_ENTRY *entries = NULL;

void InitPolyBook() {
	FILE *pFile = fopen("performance.bin", "rb");
	
	if(pFile == NULL) {
		std::cout << "Book File Not Read\n";
	} else {
		fseek(pFile, 0, SEEK_END);
		long long position = ftell(pFile);
		
		if (position < sizeof(S_POLY_BOOK_ENTRY)) {
			std::cout << "No Entries Found\n";
			return;
		}
		
		NumEntries = position / sizeof(S_POLY_BOOK_ENTRY);
		std::cout << NumEntries << " Entries Found In File\n";
		
        entries = new S_POLY_BOOK_ENTRY[NumEntries];
		rewind(pFile);
		
		size_t returnValue;
		returnValue = fread(entries, sizeof(S_POLY_BOOK_ENTRY), NumEntries, pFile);
		std::cout << "fread() " << returnValue << " Entries Read in from file\n";
	}
}

void CleanPolyBook() {
    if (entries != NULL) {
	    delete[] entries;
    }
}

bool HasPawnForCapture(const S_BOARD *board) {
	int sqWithPawn = 0;
	int targetPiece = (board->side == WHITE) ? wP : bP;

	if (board->enPas != NO_SQ) {
		if (board->side == WHITE) {
			sqWithPawn = board->enPas - 10;
		} else {
			sqWithPawn = board->enPas + 10;
		}
		
		if (board->pieces[sqWithPawn + 1] == targetPiece) {
			return TRUE;
		} else if(board->pieces[sqWithPawn - 1] == targetPiece) {
			return TRUE;
		} 
	}
	return FALSE;
}

U64 PolyKeyFromBoard(const S_BOARD *board) {
	U64 finalKey = 0;
	int offset = 0;
	
	for (int sq = 0; sq < BRD_SQ_NUM; ++sq) {
		int piece = board->pieces[sq];
		if (piece != NO_SQ && piece != EMPTY && piece != OFFBOARD) {
			ASSERT(piece >= wP && piece <= bK);
			int polyPiece = PolyKindOfPiece[piece];
			int rank = RanksBrd[sq];
			int file = FilesBrd[sq];
			finalKey ^= Random64Poly[(64 * polyPiece) + (8 * rank) + file];
		}
	}
	
	// castling
	offset = 768;
	
	if (board->castlePerm & WKCA) finalKey ^= Random64Poly[offset + 0];
	if (board->castlePerm & WQCA) finalKey ^= Random64Poly[offset + 1];
	if (board->castlePerm & BKCA) finalKey ^= Random64Poly[offset + 2];
	if (board->castlePerm & BQCA) finalKey ^= Random64Poly[offset + 3];
	
	// enpassant
	offset = 772;
	if (HasPawnForCapture(board) == TRUE) {
		int file = FilesBrd[board->enPas];
		finalKey ^= Random64Poly[offset + file];
	}
	
	if (board->side == WHITE) {
		finalKey ^= Random64Poly[780];
	}
	return finalKey;
}