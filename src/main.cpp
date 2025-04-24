#include "types.h"
#include "init.h"
#include "bitboards.h"
#include "board.h"
#include "data.h"
#include "attack.h"
#include "search.h"
#include "uci.h"

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define FEN4 "r1bqk2r/ppp2ppp/2n2n2/3pp1B1/1bB1P3/2NP1N2/PPP2PPP/R2QK2R b KQkq - 1 6"
#define FEN5 "r1b4r/ppppqkpp/2n5/4P3/4n3/2Q2N2/P4PPP/R1B2RK1 w - - 0 13"
#define FEN6 "r1b3kr/2p5/1pn1q2p/p2pP1p1/8/P4NQP/1B3PP1/R4RK1 w - d6 0 20"

// illegal FEN - black has 2 kings - error when turn on debug
#define FEN7 "rkb3kr/2p5/1pn1q2p/p2pP1p1/8/P4NQP/1B3PP1/R4RK1 w - d6 0 20"

// illegal FEN - white has 9 pawns - error when turn on debug
#define FEN8 "r1b3kr/2p5/1pn1q2p/p2pP1p1/8/PPP2NQP/PB3PPP/R4RK1 w - d6 0 20"

// illegal FEN - only 2 queens - test attacked squares
#define FEN9 "8/3q4/8/8/4Q3/8/8/8 w - - 0 2"

// illegal FEN - only 2 queens, 2 pawns - test attacked squares
#define FEN10 "8/3q1p2/8/5P2/4Q3/8/8/8 w - - 0 2"

using namespace std;

void ShowSqAttackedBySide(const int side, const S_BOARD *pos)
{
    std::cout << "Squares attacked by " << SideChar[side] << '\n';
    for (Rank rank = RANK_8; rank >= RANK_1; --rank)
    {
        for (File file = FILE_A; file <= FILE_H; ++file)
        {
            if (SqAttacked(FR2SQ(file, rank), side, pos))
            {
                std::cout << 'X';
            }
            else
            {
                std::cout << '-';
            }
        }
        std::cout << '\n';
    }
    std::cout << "\n\n";
}

int main()
{
    AllInit();

    // int num = 2, nuts = 4;
    // ASSERT(num == nuts);

    // Part 7
    //  for (int index = 0; index < BRD_SQ_NUM; index++) {
    //      if (index % 10 == 0)
    //          std::cout << '\n';
    //      printf("%5d", Sq120ToSq64[index]);
    //  }

    // std::cout << '\n';
    // std::cout << '\n';

    // for (int index = 0; index < 64; ++index) {
    //     if (index % 8 == 0)
    //         std::cout << '\n';
    //     printf("%5d", Sq64ToSq120[index]);
    // }

    // Part 8

    // U64 playBitBoard = 0ULL;

    // cout << "Start:\n";
    // PrintBitBoard(playBitBoard);

    // playBitBoard |= (1ULL << SQ64(D2));
    // cout << "D2 added:\n";
    // PrintBitBoard(playBitBoard);

    // playBitBoard |= (1ULL << SQ64(B7));
    // cout << "B7 added:\n";
    // PrintBitBoard(playBitBoard);

    // playBitBoard |= (1ULL << SQ64(G7));
    // cout << "G7 added:\n";
    // PrintBitBoard(playBitBoard);

    // playBitBoard |= (1ULL << SQ64(F6));
    // cout << "F6 added:\n";
    // PrintBitBoard(playBitBoard);

    // playBitBoard |= (1ULL << SQ64(H4));
    // cout << "H4 added:\n";
    // PrintBitBoard(playBitBoard);

    // Part9
    //  U64 playBitBoard = 0ULL;

    // playBitBoard |= (1ULL << SQ64(D2));
    // playBitBoard |= (1ULL << SQ64(D3));
    // playBitBoard |= (1ULL << SQ64(D4));

    // cout << "D2: " << SQ64(D2) << '\n';
    // PrintBitBoard(playBitBoard);

    // int count =  CNT(playBitBoard);
    // cout << "Count: " << count << '\n';

    // int index = POP(&playBitBoard);
    // cout << "Index: " << index << '\n';
    // PrintBitBoard(playBitBoard);

    // count = CNT(playBitBoard);
    // cout << "Count: " << count << '\n';

    // int sq64 = 0;
    // while (playBitBoard) {
    //     sq64 = POP(&playBitBoard);
    //     cout << "Popped: " << sq64 << '\n';
    //     PrintBitBoard(playBitBoard);
    // }

    // Part 10

    // U64 playBitBoard = 0ULL;
    // for (int i = 0; i < 64; ++i) {
    // cout << "Index: " << i << '\n';
    // PrintBitBoard(SetMask[i]);
    // cout << '\n';

    //     cout << "Index: " << i << '\n';
    //     PrintBitBoard(ClearMask[i]);
    //     cout << '\n';
    // }

    // SETBIT(&playBitBoard, 61);
    // PrintBitBoard(playBitBoard);

    // SETBIT(&playBitBoard, 11);
    // PrintBitBoard(playBitBoard);

    // CLRBIT(&playBitBoard, 61);
    // PrintBitBoard(playBitBoard);

    // Part 11
    //  int PieceOne = rand();
    //  int PieceTwo = rand();
    //  int PieceThree = rand();
    //  int PieceFour = rand();

    // cout << hex << uppercase << PieceOne << '\n';
    // cout << hex << uppercase << PieceTwo << '\n';
    // cout << hex << uppercase << PieceThree << '\n';
    // cout << hex << uppercase << PieceFour << '\n';

    // int Key = PieceOne ^ PieceTwo ^ PieceFour;
    // int TempKey = PieceTwo;
    // TempKey ^= PieceThree;
    // TempKey ^= PieceFour;
    // TempKey ^= PieceOne;

    // cout << "Key: " << hex << uppercase << Key << '\n';
    // cout << "TempKey: " << hex << uppercase << TempKey << '\n';
    // cout << '\n';

    // TempKey ^= PieceThree;
    // cout << "(Three out) TempKey: " << hex << uppercase << TempKey << '\n';

    // TempKey ^= PieceThree;
    // cout << "(Three in again) TempKey: " << hex << uppercase << TempKey << '\n';

    // Part 12
    //  for (int i = 1; i <= 10; ++i)
    //      cout << RAND_15 << ' ' << RAND_64 << '\n';

    // Part 13
    //  S_BOARD board;
    //  ResetBoard(&board);

    // Part 17->18
    //  S_BOARD board[1];

    // ParseFen(START_FEN, board);
    // PrintBoard(board);

    // ParseFen(FEN1, board);
    // PrintBoard(board);

    // ParseFen(FEN2, board);
    // PrintBoard(board);

    // ParseFen(FEN3, board);
    // PrintBoard(board);

    // ParseFen(FEN4, board);
    // PrintBoard(board);

    // ParseFen(FEN5, board);
    // PrintBoard(board);

    // std::cout << WHITE << ' ' << BLACK << ' ' << BOTH << '\n';

    // std::cout << "White - Black - BOTH big piece:\n";
    // std::cout << board->bigPiece[WHITE] << '\n';
    // std::cout << board->bigPiece[BLACK] << '\n';
    // std::cout << std::dec << board->bigPiece[BOTH] << '\n';

    // std::cout << "White - Black - BOTH major piece:\n";
    // std::cout << board->majorPiece[WHITE] << '\n';
    // std::cout << board->majorPiece[BLACK] << '\n';
    // std::cout << std::dec << board->majorPiece[BOTH] << '\n';

    // std::cout << "White - Black - BOTH minor piece:\n";
    // std::cout << board->minorPiece[WHITE] << '\n';
    // std::cout << board->minorPiece[BLACK] << '\n';
    // std::cout << std::dec << board->minorPiece[BOTH] << '\n';

    // std::cout << "White - Black king square:\n";
    // std::cout << std::dec << board->kingSquare[WHITE] << '\n';
    // std::cout << std::dec << board->kingSquare[BLACK] << '\n';

    // std::cout << "castlePerm - enPas:\n";
    // std::cout << std::dec << board->castlePerm << '\n';
    // std::cout << std::dec << board->enPas << '\n';

    // Part 20
    //  S_BOARD board[1];
    //  ParseFen(FEN6, board);
    //  PrintBoard(board);
    //  // board->posKey ^= SideKey;
    //  ASSERT(CheckBoard(board));

    // std::cout << '\n';
    // std::cout << "white pawns bitboard:\n";
    // PrintBitBoard(board->pawnsBB[WHITE]);

    // std::cout << '\n';
    // std::cout << "black pawns bitboard:\n";
    // PrintBitBoard(board->pawnsBB[BLACK]);

    // std::cout << '\n';
    // std::cout << "all pawns bitboard:\n";
    // PrintBitBoard(board->pawnsBB[BOTH]);

    // std::cout << '\n';
    // std::cout << "white king bitboard:\n";
    // PrintBitBoard(board->kingsBB[WHITE]);

    // std::cout << '\n';
    // std::cout << "black king bitboard:\n";
    // PrintBitBoard(board->kingsBB[BLACK]);

    // std::cout << '\n';
    // std::cout << "white rooks bitboard:\n";
    // PrintBitBoard(board->rooksBB[WHITE]);

    // std::cout << '\n';
    // std::cout << "black rooks bitboard:\n";
    // PrintBitBoard(board->rooksBB[BLACK]);

    // std::cout << '\n';
    // std::cout << "white knights bitboard:\n";
    // PrintBitBoard(board->knightsBB[WHITE]);

    // std::cout << '\n';
    // std::cout << "black knights bitboard:\n";
    // PrintBitBoard(board->knightsBB[BLACK]);

    // std::cout << '\n';
    // std::cout << "white bishops bitboard:\n";
    // PrintBitBoard(board->bishopsBB[WHITE]);

    // std::cout << '\n';
    // std::cout << "black bishops bitboard:\n";
    // PrintBitBoard(board->bishopsBB[BLACK]);

    // std::cout << '\n';
    // std::cout << "all pieces white bitboard:\n";
    // PrintBitBoard(board->allPiecesBB[WHITE]);

    // std::cout << '\n';
    // std::cout << "all pieces black bitboard:\n";
    // PrintBitBoard(board->allPiecesBB[BLACK]);

    // std::cout << '\n';
    // std::cout << "all pieces both bitboard:\n";
    // PrintBitBoard(board->allPiecesBB[BOTH]);

    // std::cout << "white pawns piece: " << board->pieceNum[wP] << '\n';
    // std::cout << "black pawns piece: " << board->pieceNum[bP] << '\n';

    // Part 23 - turn off debug for illegal FEN
    // S_BOARD board[1];
    // ParseFen(FEN10, board);
    // PrintBoard(board);
    // // ASSERT(CheckBoard(board));

    // std::cout << "\n\nWhite is attacking:\n";
    // ShowSqAttackedBySide(WHITE, board);

    // std::cout << "\n\nBlack is attacking:\n";
    // ShowSqAttackedBySide(BLACK, board);

    // return 0;

    // Test UCI
    S_BOARD pos[1];
    S_SEARCHINFO info[1];
    info->quit = FALSE;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    cout << "Welcome to Our Chessengine! Type 'uci' to start..." << endl;

    string input;
    while (true)
    {
        cout << "> ";
        if (!getline(cin, input))
            continue;
        if (input.empty())
            continue;

        if (input.rfind("uci", 0) == 0)
        {
            Uci_Loop(pos, info);
            if (info->quit)
                break;
        }
        else if (input.rfind("quit", 0) == 0)
        {
            break;
        }
    }
}