#include "init.h"
#include "movegen.h"
#include "polybook.h"
// #include "bitboards.h" //print bitboard
// #include "io.h" //print square
#include "bitboards.h"

#include <iostream>
#include <iomanip>

int Sq120ToSq64[BRD_SQ_NUM];
int Sq64ToSq120[64];

U64 SetMask[64]; //see at init.h
U64 ClearMask[64];

U64 PieceKeys[13][120];
U64 SideKey;
U64 CastleKeys[16];

int FilesBrd[BRD_SQ_NUM];
int RanksBrd[BRD_SQ_NUM];

U64 FileBBMask[8];
U64 RankBBMask[8];

U64 BlackPassedMask[64];
U64 WhitePassedMask[64];
U64 IsolatedMask[64];

void InitEvalMasks() {
    for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
        for (File file = FILE_A; file <= FILE_H; ++file) {
            int sq = rank * 8 + file;
            FileBBMask[file] |= (1ULL << sq);
            RankBBMask[rank] |= (1ULL << sq);
        }
	}

    // for (Rank rank = RANK_1; rank <= RANK_8; ++rank) {
    //     std::cout << "rank " << rank << '\n';
    //     PrintBitBoard(RankBBMask[rank]);
    //     std::cout << '\n';
    // }
    
    // for (File file = FILE_A; file <= FILE_H; ++file) {
    //     std::cout << "file " << file << '\n';
    //     PrintBitBoard(FileBBMask[file]);
    // }

    for (int sq = 0; sq < 64; ++sq) {
		int tsq = sq + 8;

        while (tsq < 64) {
            WhitePassedMask[sq] |= (1ULL << tsq);
            tsq += 8;
        }

        tsq = sq - 8;
        while (tsq >= 0) {
            BlackPassedMask[sq] |= (1ULL << tsq);
            tsq -= 8;
        }

        if (FilesBrd[SQ120(sq)] > FILE_A) {
            IsolatedMask[sq] |= FileBBMask[FilesBrd[SQ120(sq)] - 1];

            tsq = sq + 7;
            while (tsq < 64) {
                WhitePassedMask[sq] |= (1ULL << tsq);
                tsq += 8;
            }

            tsq = sq - 9;
            while (tsq >= 0) {
                BlackPassedMask[sq] |= (1ULL << tsq);
                tsq -= 8;
            }
        }

        if (FilesBrd[SQ120(sq)] < FILE_H) {
            IsolatedMask[sq] |= FileBBMask[FilesBrd[SQ120(sq)] + 1];

            tsq = sq + 9;
            while (tsq < 64) {
                WhitePassedMask[sq] |= (1ULL << tsq);
                tsq += 8;
            }

            tsq = sq - 7;
            while (tsq >= 0) {
                BlackPassedMask[sq] |= (1ULL << tsq);
                tsq -= 8;
            }
        }

        // std::cout << "square " << PrintSquare(SQ120(sq)) << '\n';
        // std::cout << "isolated mask\n";
        // PrintBitBoard(IsolatedMask[sq]);
        // std::cout << "white passed mask\n";
        // PrintBitBoard(WhitePassedMask[sq]);
        // std::cout << "black passed mask\n";
        // PrintBitBoard(BlackPassedMask[sq]);
        // std::cout << '\n';
	}
}

void InitFilesRanksBrd() {
	for(int i = 0; i < BRD_SQ_NUM; ++i) {
		FilesBrd[i] = OFFBOARD;
		RanksBrd[i] = OFFBOARD;
	}

	for (Rank rank = RANK_1; rank <= RANK_8; ++rank) {
		for (File file = FILE_A; file <= FILE_H; ++file) {
			int sq = FR2SQ(file,rank);
			FilesBrd[sq] = file;
			RanksBrd[sq] = rank;
		}
	}

    // std::cout << "\nFile\n";
    // for (int i = 0; i < BRD_SQ_NUM; ++i) {
    //     if (i % 10 == 0 && i != 0) std::cout << '\n';
    //     std::cout << std::setw(4) << FilesBrd[i];
    // }

    // std::cout << "\nRank\n";
    // for (int i = 0; i < BRD_SQ_NUM; ++i) {
    //     if (i % 10 == 0 && i != 0) std::cout << '\n';
    //     std::cout << std::setw(4) << RanksBrd[i];
    // }
}

void InitHashKeys() {
    for (int i = 0; i < 13; ++i) {
        for (int j = 0; j < 120; ++j) {
            PieceKeys[i][j] = RAND_64;
        }
    }
    SideKey = RAND_64;
    for (int i = 0; i < 16; i++) {
        CastleKeys[i] = RAND_64;
    }
}

void InitBitMasks() {
    for (int i = 0; i < SQUARE_NB; ++i) {
        SetMask[i] = (1ULL << i);
        ClearMask[i] = ~SetMask[i];
    }
}

void clearBit(U64 *bb, U64 sq) {
    *bb &= ClearMask[sq]; 
}

void setBit(U64 *bb, U64 sq) {
    *bb |= SetMask[sq];
}

void InitSq120To64(){
    for (int i = 0; i < BRD_SQ_NUM; ++i){
        Sq120ToSq64[i] = 65; //indexing them all to 65 becuz thge max is 63 (0 to 63 cases)
    }

    for (int i = 0; i < SQUARE_NB; ++i){
        Sq64ToSq120[i] = 120; // same as 65 but for the 120 array
    }

    int sq = A1;
    int sq64 = 0;
    
    for(Rank rank = RANK_1; rank <= RANK_8; ++rank){ // we browse through all the ranks
        for(File file = FILE_A; file <= FILE_H; ++file){ // .... the files
            sq = FR2SQ(file, rank); // fpor each coordinate (file, rank), we assign it a square in the 64 array
            Sq64ToSq120[sq64] = sq;
            Sq120ToSq64[sq] = sq64;
            sq64++; // at the end sq64 will represent the 64 base index array for that specific square
        }
    }
}

void InitSrandTime() {
    srand(time(NULL)); // Seed based on current time
}

void AllInit() {
    InitSrandTime(); //must before any of using rand(), can turn off for debug so each pos will have fix posKey
    InitSq120To64();
    InitBitMasks();
    InitHashKeys();
    InitFilesRanksBrd();
    InitEvalMasks();
    InitMvvLva();
    InitPolyBook();
    InitMagic();
}