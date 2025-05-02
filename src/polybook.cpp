#include "polybook.h"
#include "polykeys.h"
#include "init.h"
#include "data.h" //for print move

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

unsigned short endian_swap_u16(unsigned short x) { //big -> small endian
    x = (x>>8) | 
        (x<<8); 
    return x;
} 

unsigned int endian_swap_u32(unsigned int x) {
    x = (x>>24) | 
        ((x<<8) & 0x00FF0000) | 
        ((x>>8) & 0x0000FF00) | 
        (x<<24); 
    return x;
} 

U64 endian_swap_u64(U64 x) {
    x = (x>>56) | 
        ((x<<40) & 0x00FF000000000000) | 
        ((x<<24) & 0x0000FF0000000000) | 
        ((x<<8)  & 0x000000FF00000000) | 
        ((x>>8)  & 0x00000000FF000000) | 
        ((x>>24) & 0x0000000000FF0000) | 
        ((x>>40) & 0x000000000000FF00) | 
        (x<<56); 
    return x;
}

void ListBookMoves(U64 polyKey) {
    int start = 0;
    unsigned short move;
    for (S_POLY_BOOK_ENTRY *entry = entries; entry < entries + NumEntries; entry++) {
        if (polyKey == endian_swap_u64(entry->key)) {
            move = endian_swap_u16(entry->move);
            std::cout << std::hex << "Key:" << endian_swap_u64(entry->key)
            << std::dec << " Index:" << start
            << " Move:"
            << FileChar[(move >> 6) & 7]
            << RankChar[(move >> 9) & 7]
            << FileChar[(move >> 0) & 7]
            << RankChar[(move >> 3) & 7]
            << '\n';
        }
        start++;
    }
}

int GetBookMove(S_BOARD *board) {
	// int index = 0;
	// S_POLY_BOOK_ENTRY *entry;
	// unsigned short move;
	// const int MAXBOOKMOVES = 32;
	// int bookMoves[MAXBOOKMOVES];
	// int tempMove = NOMOVE;
	// int count = 0;
	
	U64 polyKey = PolyKeyFromBoard(board);
    std::cout << std::hex << "polykey:" << polyKey << '\n' << std::dec;
    ListBookMoves(polyKey);
	
	// for(entry = entries; entry < entries + NumEntries; entry++) {
	// 	if(polyKey == endian_swap_u64(entry->key)) {
	// 		move = endian_swap_u16(entry->move);
	// 		tempMove = ConvertPolyMoveToInternalMove(move, board);
	// 		if(tempMove != NOMOVE) {
	// 			bookMoves[count++] = tempMove;
	// 			if(count > MAXBOOKMOVES) break;
	// 		}
	// 	}
	// }
	
	// if(count != 0) {
	// 	int randMove = rand() % count;
	// 	return bookMoves[randMove];
	// } else {
	// 	return NOMOVE;
	// }

    return NOMOVE;
}
