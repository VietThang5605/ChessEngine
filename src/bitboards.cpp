#include "bitboards.h"
#include "init.h"
#include "movegen.h"
#include "validate.h"

#include <iostream>

const int BitTable[64] = {
    63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2, 
    51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
     26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
      58, 20, 37, 17, 36, 8
}; //

const int RookShifts[64] = { 
    52, 52, 52, 52, 52, 52, 52, 52, 
    53, 53, 53, 54, 53, 53, 54, 53, 
    53, 54, 54, 54, 53, 53, 54, 53, 
    53, 54, 53, 53, 54, 54, 54, 53, 
    52, 54, 53, 53, 53, 53, 54, 53, 
    52, 53, 54, 54, 53, 53, 54, 53, 
    53, 54, 54, 54, 53, 53, 54, 53, 
    52, 53, 53, 53, 53, 53, 53, 52 
};

const int BishopShifts[64] = { 
    58, 60, 59, 59, 59, 59, 60, 58, 
    60, 59, 59, 59, 59, 59, 59, 60, 
    59, 59, 57, 57, 57, 57, 59, 59, 
    59, 59, 57, 55, 55, 57, 59, 59, 
    59, 59, 57, 55, 55, 57, 59, 59, 
    59, 59, 57, 57, 57, 57, 59, 59, 
    60, 60, 59, 59, 59, 59, 60, 60, 
    58, 60, 59, 59, 59, 59, 59, 58 
};

const U64 RookMagics[64] = { 
    468374916371625120ULL, 18428729537625841661ULL, 2531023729696186408ULL, 6093370314119450896ULL, 13830552789156493815ULL, 16134110446239088507ULL, 12677615322350354425ULL, 5404321144167858432ULL, 
    2111097758984580ULL, 18428720740584907710ULL, 17293734603602787839ULL, 4938760079889530922ULL, 7699325603589095390ULL, 9078693890218258431ULL, 578149610753690728ULL, 9496543503900033792ULL, 
    1155209038552629657ULL, 9224076274589515780ULL, 1835781998207181184ULL, 509120063316431138ULL, 16634043024132535807ULL, 18446673631917146111ULL, 9623686630121410312ULL, 4648737361302392899ULL, 
    738591182849868645ULL, 1732936432546219272ULL, 2400543327507449856ULL, 5188164365601475096ULL, 10414575345181196316ULL, 1162492212166789136ULL, 9396848738060210946ULL, 622413200109881612ULL, 
    7998357718131801918ULL, 7719627227008073923ULL, 16181433497662382080ULL, 18441958655457754079ULL, 1267153596645440ULL, 18446726464209379263ULL, 1214021438038606600ULL, 4650128814733526084ULL, 
    9656144899867951104ULL, 18444421868610287615ULL, 3695311799139303489ULL, 10597006226145476632ULL, 18436046904206950398ULL, 18446726472933277663ULL, 3458977943764860944ULL, 39125045590687766ULL, 
    9227453435446560384ULL, 6476955465732358656ULL, 1270314852531077632ULL, 2882448553461416064ULL, 11547238928203796481ULL, 1856618300822323264ULL, 2573991788166144ULL, 4936544992551831040ULL, 
    13690941749405253631ULL, 15852669863439351807ULL, 18302628748190527413ULL, 12682135449552027479ULL, 13830554446930287982ULL, 18302628782487371519ULL, 7924083509981736956ULL, 4734295326018586370ULL 
};

const U64 BishopMagics[64] = { 
    16509839532542417919ULL, 14391803910955204223ULL, 1848771770702627364ULL, 347925068195328958ULL, 5189277761285652493ULL, 3750937732777063343ULL, 18429848470517967340ULL, 17870072066711748607ULL, 
    16715520087474960373ULL, 2459353627279607168ULL, 7061705824611107232ULL, 8089129053103260512ULL, 7414579821471224013ULL, 9520647030890121554ULL, 17142940634164625405ULL, 9187037984654475102ULL, 
    4933695867036173873ULL, 3035992416931960321ULL, 15052160563071165696ULL, 5876081268917084809ULL, 1153484746652717320ULL, 6365855841584713735ULL, 2463646859659644933ULL, 1453259901463176960ULL, 
    9808859429721908488ULL, 2829141021535244552ULL, 576619101540319252ULL, 5804014844877275314ULL, 4774660099383771136ULL, 328785038479458864ULL, 2360590652863023124ULL, 569550314443282ULL, 
    17563974527758635567ULL, 11698101887533589556ULL, 5764964460729992192ULL, 6953579832080335136ULL, 1318441160687747328ULL, 8090717009753444376ULL, 16751172641200572929ULL, 5558033503209157252ULL, 
    17100156536247493656ULL, 7899286223048400564ULL, 4845135427956654145ULL, 2368485888099072ULL, 2399033289953272320ULL, 6976678428284034058ULL, 3134241565013966284ULL, 8661609558376259840ULL, 
    17275805361393991679ULL, 15391050065516657151ULL, 11529206229534274423ULL, 9876416274250600448ULL, 16432792402597134585ULL, 11975705497012863580ULL, 11457135419348969979ULL, 9763749252098620046ULL, 
    16960553411078512574ULL, 15563877356819111679ULL, 14994736884583272463ULL, 9441297368950544394ULL, 14537646123432199168ULL, 9888547162215157388ULL, 18140215579194907366ULL, 18374682062228545019ULL };

U64 RookMask[64];
U64 BishopMask[64];

U64 *RookAttacks[64];
U64 *BishopAttacks[64];

int PopBit(U64 *bb) { // takestakes the first bit in the least significant bit in the bitboard
    U64 b = *bb ^ (*bb -1); // and return the index of tihs bit and set it to 0
    unsigned int fold = (unsigned) ((b & 0xffffffff) ^(b >> 32));
    *bb &= (*bb -1);
    return BitTable[(fold * 0x783a9b23) >> 26];
}

int CountBits(U64 b) { //counts and return the number of 1s in the bitboard
    int r;
    for(r = 0; b; r++, b &= b-1);
    return r;
}

void PrintBitBoard(U64 bb) {
    U64 shiftMe = 1ULL; //long long cuz we need more than 32 bits

    int rank =0;
    int file = 0;
    int sq = 0;
    int sq64 = 0;

    std::cout << '\n';
    for(rank = RANK_8; rank >= RANK_1; --rank){ //starting at the 8th rank and file A
        for (file = FILE_A; file <= FILE_H; ++file){ //so on to 1st rank and file H
            sq = FR2SQ(file, rank); // the 120 base index
            sq64 = SQ64(sq); // need the 64 base index cuz we gon use our 64 bit number
            if((shiftMe << sq64) & bb) //so if there's a bit on there, print ax X
                std::cout << 'X';
            else   
                std::cout << '-'; //otherwise print a minus
        }
        std::cout << '\n'; 
    }
    std::cout << '\n';
}

U64 GetSliderAttacks(int square, U64 blockers, bool ortho) {
    return (ortho ? GetRookAttacks(square, blockers) : GetBishopAttacks(square, blockers));
}

static U64 GetRookAttacks(int square, U64 blockers) {
    U64 key = ((blockers & RookMask[square]) * RookMagics[square]) >> RookShifts[square];
	return RookAttacks[square][key];
}

static U64 GetBishopAttacks(int square, U64 blockers) {
    U64 key = ((blockers & BishopMask[square]) * BishopMagics[square]) >> BishopShifts[square];
	return BishopAttacks[square][key];
}

static void CreateMovementMask(int squareIndex, bool rook) { //squareIndex: 0..63
    ASSERT(0 <= squareIndex && squareIndex < 64);
    U64 mask = 0ULL;
    int startSquare = SQ120(squareIndex); //convert to 120 board
    if (rook) {
        for (int dir = 0; dir < NumDir[wR]; ++dir) { //Rook
            for (int dst = 1; dst < 8; ++dst) {
                int nextSquare = startSquare + PieceDir[wR][dir] * dst;
                int next2Square = startSquare + PieceDir[wR][dir] * (dst + 1);

                if (next2Square <= 0 || next2Square >= BRD_SQ_NUM)
                    break;

                if (!SqOnBoard(nextSquare)) {
                    break;
                }
                if (!SqOnBoard(next2Square)) {
                    break;
                }

                setBit(&mask, SQ64(nextSquare));
            }
        }
        RookMask[squareIndex] = mask;

    } else {
        for (int dir = 0; dir < NumDir[wB]; ++dir) { //Bishop
            for (int dst = 1; dst < 8; ++dst) {
                int nextSquare = startSquare + PieceDir[wB][dir] * dst;
                int next2Square = startSquare + PieceDir[wB][dir] * (dst + 1);
                
                if (next2Square <= 0 || next2Square >= BRD_SQ_NUM)
                    break;

                if (!SqOnBoard(nextSquare)) {
                    break;
                }
                if (!SqOnBoard(next2Square)) {
                    break;
                }

                setBit(&mask, SQ64(nextSquare));
            }
        }
        BishopMask[squareIndex] = mask;

    }
}

static U64 LegalMoveBitboardFromBlockers(int square, U64 blockerBitboard, bool rook) {
    U64 bitboard = 0ULL;
    int startSquare = SQ120(square);

    if (rook) {
        for (int dir = 0; dir < NumDir[wR]; ++dir) {
            for (int dst = 1; dst < 8; ++dst) {
                int nextSquare = startSquare + PieceDir[wR][dir] * dst;
                if (SqOnBoard(nextSquare)) {
                    setBit(&bitboard, SQ64(nextSquare));
                    if ((blockerBitboard >> SQ64(nextSquare)) & 1) { //if blocker contains this square then break
                        break;
                    }
                } else {
                    break;
                }
            }
        }

    } else {
        for (int dir = 0; dir < NumDir[wB]; ++dir) {
            for (int dst = 1; dst < 8; ++dst) {
                int nextSquare = startSquare + PieceDir[wB][dir] * dst;
                if (SqOnBoard(nextSquare)) {
                    setBit(&bitboard, SQ64(nextSquare));
                    if ((blockerBitboard >> SQ64(nextSquare)) & 1) { //if blocker contains this square then break
                        break;
                    }
                } else {
                    break;
                }
            }
        }
    }

    return bitboard;
}

static void CreateTable(int square, bool rook, U64 magic, int leftShift) {
    int numBits = 64 - leftShift;
    int lookupSize = 1 << numBits;
    if (rook) {
        RookAttacks[square] = NULL;
        RookAttacks[square] = new U64[lookupSize];
        for (int i = 0; i < lookupSize; ++i) {
            RookAttacks[square][i] = 0ULL;
        }

        U64 movementMask = RookMask[square];

        //create all blockerBitboard
        int moveSquareIndices[16];
        int countBit = 0;
        for (int i = 0; i < 64; ++i) {
            if ((movementMask >> i) & 1) {
                moveSquareIndices[countBit++] = i;
            }
        }

        int numPatterns = 1 << countBit;
        U64* blockerPatterns = new U64[numPatterns];

        for (int patternIndex = 0; patternIndex < numPatterns; ++patternIndex) {
            blockerPatterns[patternIndex] = 0ULL; //important

            for (int bitIndex = 0; bitIndex < countBit; ++bitIndex) {	
                if ((patternIndex >> bitIndex) & 1) {
                    blockerPatterns[patternIndex] |= (1ULL << moveSquareIndices[bitIndex]);
                }
            }
        }

        // int countCollision = 0;
        for (int i = 0; i < numPatterns; ++i) {
            U64 pattern = blockerPatterns[i];
            U64 index = (pattern * magic) >> leftShift;
            // if (index >= lookupSize) {
            //     std::cout << "ERROR\n";
            // }
            // if (RookAttacks[square][index] != 0ULL && RookAttacks[square][index] != LegalMoveBitboardFromBlockers(square, pattern, rook)) {
            //     countCollision++;
            //     // std::cout << "Square: " << square << " index: " << index << '\n';
            // }
            U64 moves = LegalMoveBitboardFromBlockers(square, pattern, rook);
			RookAttacks[square][index] = moves;
        }
        // std::cout << "Square: " << square << " countCollsion: " << countCollision << '\n';

        delete[] blockerPatterns;
    } else {
        BishopAttacks[square] = NULL;
        BishopAttacks[square] = new U64[lookupSize];
        for (int i = 0; i < lookupSize; ++i) {
            BishopAttacks[square][i] = 0ULL;
        }

        U64 movementMask = BishopMask[square];

        //create all blockerBitboard
        int moveSquareIndices[16];
        int countBit = 0;
        for (int i = 0; i < 64; ++i) {
            if ((movementMask >> i) & 1) {
                moveSquareIndices[countBit++] = i;
            }
        }

        int numPatterns = 1 << countBit;
        U64* blockerPatterns = new U64[numPatterns];

        for (int patternIndex = 0; patternIndex < numPatterns; ++patternIndex) {
            blockerPatterns[patternIndex] = 0ULL; //important

            for (int bitIndex = 0; bitIndex < countBit; ++bitIndex) {	
                int bit = (patternIndex >> bitIndex) & 1;
                blockerPatterns[patternIndex] |= ((U64)bit) << moveSquareIndices[bitIndex];
            }
        }

        // int countCollision = 0;
        for (int i = 0; i < numPatterns; ++i) {
            U64 pattern = blockerPatterns[i];
            U64 index = (pattern * magic) >> leftShift;
            // if (BishopAttacks[square][index] != 0ULL && BishopAttacks[square][index] != LegalMoveBitboardFromBlockers(square, pattern, rook)) {
            //     countCollision++;
            //     // std::cout << "Square: " << square << " index: " << index << '\n';
            // }
            U64 moves = LegalMoveBitboardFromBlockers(square, pattern, rook);
			BishopAttacks[square][index] = moves;
        }
        // std::cout << "Square: " << square << " countCollsion: " << countCollision << '\n';

        delete[] blockerPatterns;
    }
}

void InitMagic() {
    for (int squareIndex = 0; squareIndex < 64; ++squareIndex) {
        CreateMovementMask(squareIndex, TRUE);
        CreateMovementMask(squareIndex, FALSE);
    }

    for (int i = 0; i < 64; i++) {
        CreateTable(i, TRUE, RookMagics[i], RookShifts[i]);
        CreateTable(i, FALSE, BishopMagics[i], BishopShifts[i]);
    }

    std::cout << "Done Init Magic\n";

    // for (int i = 0; i <= 20; i++) {
    //     std::cout << "Rook movement at square: " << i << '\n';
    //     PrintBitBoard(RookMask[i]);
    //     std::cout << "\n\n";

    //     std::cout << "Bishop movement at square: " << i << '\n';
    //     PrintBitBoard(BishopMask[i]);
    //     std::cout << "\n\n";
    // }

    // std::cout << "Rook movement at square: " << 10 << '\n';
    // PrintBitBoard(RookMask[10]);
    // std::cout << "\n\n";

    // for (int blockerBitboard = 0; blockerBitboard < 50; blockerBitboard++) {
    //     std::cout << "Rook attack at square: " << 10 << " with blocker: " << blockerBitboard << '\n';
    //     PrintBitBoard(RookAttacks[10][blockerBitboard]);
    //     std::cout << "\n\n";
    // }
}

void FreeMagic() {
    for (int i = 0; i < 64; i++) {
        if (RookAttacks[i] != NULL) {
            delete[] RookAttacks[i];
        }
        if (BishopAttacks[i] != NULL) {
            delete[] BishopAttacks[i];
        }
    }
    std::cout << "Done Free Magic\n";
}