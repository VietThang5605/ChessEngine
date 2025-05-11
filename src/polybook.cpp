#include "polybook.h"
#include "polykeys.h"
#include "init.h"
#include "io.h"
#include "ucioption.h"
#include "data.h" //for print move

const int PolyKindOfPiece[13] = {
	-1, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10
};

long long NumEntries = 0;

S_POLY_BOOK_ENTRY *entries = NULL;

void InitPolyBook() {
    EngineOptions->UseBook = FALSE;

	FILE *pFile = fopen("komodo.bin", "rb");
	
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

        if (NumEntries > 0) {
			EngineOptions->UseBook = TRUE;
		}
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

static unsigned short endian_swap_u16(unsigned short x) { //big -> small endian
    x = (x>>8) | 
        (x<<8); 
    return x;
} 

static unsigned int endian_swap_u32(unsigned int x) {
    x = (x>>24) | 
        ((x<<8) & 0x00FF0000) | 
        ((x>>8) & 0x0000FF00) | 
        (x<<24); 
    return x;
} 

static U64 endian_swap_u64(U64 x) {
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

static int ConvertPolyMoveToInternalMove(unsigned short polyMove, S_BOARD *board) {
	int fromFile = (polyMove >> 6) & 7;
	int fromRank = (polyMove >> 9) & 7;
	int toFile = (polyMove >> 0) & 7;
	int toRank = (polyMove >> 3) & 7;
	int promotedPiece = (polyMove >> 12) & 7;
	
	char moveString[6];
	if (promotedPiece == 0) {
		snprintf(moveString, sizeof(moveString), "%c%c%c%c",
            FileChar[fromFile],
            RankChar[fromRank],
            FileChar[toFile],
            RankChar[toRank]);

	} else {
		char promChar = 'q';
		switch (promotedPiece) {
			case 1: promChar = 'n'; break;
			case 2: promChar = 'b'; break;
			case 3: promChar = 'r'; break;
		}
		snprintf(moveString, sizeof(moveString), "%c%c%c%c%c",
            FileChar[fromFile],
            RankChar[fromRank],
            FileChar[toFile],
            RankChar[toRank],
            promChar);
	}
	
	return ParseMove(moveString, board);
}

static int KeyLowerBoundIndex(U64 polyKey) {
	int index = -1;
	int leftBound = 0, rightBound = NumEntries - 1;
	int count = 0;
	while (leftBound <= rightBound) {
		count++;
		int mid = leftBound + ((rightBound - leftBound) / 2);
		if (endian_swap_u64((entries + mid)->key) >= polyKey) {
			index = mid;
			rightBound = mid - 1;
		} else {
			leftBound = mid + 1;
		}
	}
	if (index != -1 && endian_swap_u64((entries + index)->key) != polyKey) {
		index = -1;
	}
	return index;
}

int GetBookMove(S_BOARD *board) {
    unsigned short move;
    const int MAXBOOKMOVES = 32;
    int bookMoves[MAXBOOKMOVES];
    int tempMove = NOMOVE;
    int count = 0;

    U64 polyKey = PolyKeyFromBoard(board);

	int startIndex = KeyLowerBoundIndex(polyKey);
	if (startIndex == -1) {
		return NOMOVE;
	}

    for (S_POLY_BOOK_ENTRY *entry = (entries + startIndex); entry < (entries + NumEntries); entry++) {
        if (polyKey == endian_swap_u64(entry->key)) {
            move = endian_swap_u16(entry->move);
            tempMove = ConvertPolyMoveToInternalMove(move, board);
            if (tempMove != NOMOVE) {
                bookMoves[count++] = tempMove;
                if (count >= MAXBOOKMOVES) {
                    break;
                }
            }
        }
    }

    if (count != 0) {
		int randMove = rand() % count;
		return bookMoves[randMove];
	} else {
		return NOMOVE;
	}
}