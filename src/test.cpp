// #include "types.h"
// #include "init.h"
// #include "bitboards.h"
// #include "board.h"
// #include "data.h"
// #include "attack.h"
// #include "io.h"
// #include "movegen.h"
// #include "makemove.h"
// #include "perft.h"
// #include "search.h"
// #include "pvtable.h"
// #include "misc.h"
// #include "uci.h"
// #include "polybook.h"
// #include "ucioption.h"

// #include <iostream>
// #include <iomanip>
// #include <cstdlib>
// #include <ctime>
// #include <limits>

// #define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
// #define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
// #define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
// #define FEN4 "r1bqk2r/ppp2ppp/2n2n2/3pp1B1/1bB1P3/2NP1N2/PPP2PPP/R2QK2R b KQkq - 1 6"
// #define FEN5 "r1b4r/ppppqkpp/2n5/4P3/4n3/2Q2N2/P4PPP/R1B2RK1 w - - 0 13"
// #define FEN6 "r1b3kr/2p5/1pn1q2p/p2pP1p1/8/P4NQP/1B3PP1/R4RK1 w - d6 0 20"

// //illegal FEN - black has 2 kings - error when turn on debug
// #define FEN7 "rkb3kr/2p5/1pn1q2p/p2pP1p1/8/P4NQP/1B3PP1/R4RK1 w - d6 0 20"

// //illegal FEN - white has 9 pawns - error when turn on debug
// #define FEN8 "r1b3kr/2p5/1pn1q2p/p2pP1p1/8/PPP2NQP/PB3PPP/R4RK1 w - d6 0 20"

// //illegal FEN - only 2 queens - test attacked squares
// #define FEN9 "8/3q4/8/8/4Q3/8/8/8 w - - 0 2"

// //illegal FEN - only 2 queens, 2 pawns - test attacked squares
// #define FEN10 "8/3q1p2/8/5P2/4Q3/8/8/8 w - - 0 2"

// //FEN for testing pawn moves
// #define FEN_WHITE_PAWNMOVES "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P5/RNBQKBNR w KQkq e6 0 1"
// #define FEN_BLACK_PAWNMOVES "rnbqkb1r/p3p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"

// //FEN for testing another piece moves
// #define FEN_BLACK_KNIGHTSKINGS "5k2/1n6/4n3/6N1/8/3N4/8/5K2 b - - 0 1"
// #define FEN_WHITE_KNIGHTSKINGS "5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1"

// #define FEN_WHITE_ROOKS "6k1/8/5r2/8/1nR5/5N2/8/6K1 w - - 0 1"
// #define FEN_BLACK_ROOKS "6k1/8/5r2/8/1nR5/5N2/8/6K1 b - - 0 1"

// #define FEN_WHITE_QUEENS "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 w - - 0 1"
// #define FEN_BLACK_QUEENS "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 b - - 0 1"

// #define FEN_WHITE_BISHOPS "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 w - - 0 1"
// #define FEN_BLACK_BISHOPS "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 b - - 0 1"

// #define FEN_WHITE_CASTLE1 "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
// #define FEN_BLACK_CASTLE1 "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1"

// #define FEN_WHITE_CASTLE2 "3rk2r/8/8/8/8/8/6p1/R3K2R w KQk - 0 1"
// #define FEN_BLACK_CASTLE2 "3rk2r/8/8/8/8/8/6p1/R3K2R b KQk - 0 1"

// #define FEN_WHITE_CASTLE3 "1r2k2r/8/8/8/8/8/7p/R3K2R w KQk - 0 1"
// #define FEN_BLACK_CASTLE3 "1r2k2r/8/8/8/8/8/7p/R3K2R b KQk - 0 1"

// #define FEN_WHITE_CASTLE4 "1r2k1Nr/8/8/b7/8/8/7p/R3K2R w KQk - 0 1"
// #define FEN_BLACK_CASTLE4 "1r2k1Nr/8/8/b7/8/8/7p/R3K2R b KQk - 0 1"

// #define FEN_WHITE_CASTLE5 "1r2k1Nr/8/8/8/2b5/8/7p/R3K2R w KQk - 0 1"
// #define FEN_BLACK_CASTLE5 "1r2k1Nr/8/8/8/2b5/8/7p/R3K2R b KQk - 0 1"

// #define FEN11 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

// //FEN for perft testing
// #define FEN_PERFT_2 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
// #define FEN_PERFT_3 "4k3/8/8/8/8/8/8/4K2R w K - 0 1"
// #define FEN_PERFT_4 "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1"
// #define FEN_PERFT_8 "r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1"
// #define FEN_PERFT_13 "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
// #define FEN_PERFT_14 "r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1"
// #define FEN_PERFT_17 "1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1"
// #define FEN_PERFT_19 "r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1"
// #define FEN_PERFT_20 "4k3/8/8/8/8/8/8/4K2R b K - 0 1"
// #define FEN_PERFT_30 "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1"
// #define FEN_PERFT_40 "K7/8/2n5/1n6/8/8/8/k6N w - - 0 1"
// #define FEN_PERFT_47 "B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1"
// #define FEN_PERFT_64 "K7/8/8/3Q4/4q3/8/8/7k b - - 0 1"
// #define FEN_PERFT_111 "3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1"
// #define FEN_PERFT_120 "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1"
// #define FEN_PERFT_126 "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1"

// #define FEN_PARSEMOVE_1 "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"

// //FEN for testing search
// #define FEN_WAC1 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 1"
// #define FEN_WAC2 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

// #define FEN_MATEIN2_1 "r1bq2r1/b4pk1/p1pp1p2/1p2pP2/1P2P1PB/3P4/1PPQ2P1/R3K2R w - - 0 1"
// #define FEN_MATEIN2_2 "r2qk2r/pb4pp/1n2Pb2/2B2Q2/p1p5/2P5/2B2PPP/RN2R1K1 w - - 1 0"
// #define FEN_MATEIN2_3 "kbK5/pp6/1P6/8/8/8/8/R7 w - - 0 1"
// #define FEN_MATEIN2_4 "r1b2k1r/ppp1bppp/8/1B1Q4/5q2/2P5/PPP2PPP/R3R1K1 w - - 1 0"
// #define FEN_MATEIN2_5 "r3q1k1/p6p/2bP1rpb/1pB2p1Q/2nN1P2/P1n3PB/1P3K1P/RR6 b - - 1 22"

// #define FEN_MATEIN3_1 "2r3k1/p4p2/3Rp2p/1p2P1pK/8/1P4P1/P3Q2P/1q6 b - - 0 1"
// #define FEN_MATEIN3_2 "R6R/1r3pp1/4p1kp/3pP3/1r2qPP1/7P/1P1Q3K/8 w - - 1 0"
// #define FEN_MATEIN3_3 "r3k2r/ppp2Npp/1b5n/4p2b/2B1P2q/BQP2P2/P5PP/RN5K w kq - 1 0"
// #define FEN_MATEIN3_4 "rnbqk1nr/2p2pp1/p3p3/2bp3p/7P/2N2P1N/1PPPP1P1/R1BQKB1R b Kkq - 2 8"
// #define FEN_MATEIN3_5 "rn2k2r/p3q2p/B1p1bp1n/P5pP/1b6/5N2/1PPN1PP1/R1BQK2R b KQkq - 4 15"

// #define FEN_MATEIN5_1 "2rq4/3r1kb1/p2R4/1PB1np1p/1P6/5pP1/7P/1N1QRK2 w - - 3 43"
// #define FEN_MATEIN5_2 "r1Q5/5bk1/3p1pp1/3n1p1p/7P/4PNPB/1r3PRK/R7 w - - 1 46"

// using namespace std;

// void ShowSqAttackedBySide(const int side, const S_BOARD *pos) {
//     std::cout << "Squares attacked by " << SideChar[side] << '\n';
//     for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
//         for (File file = FILE_A; file <= FILE_H; ++file) {
//             if (SqAttacked(FR2SQ(file, rank), side, pos)) {
//                 std::cout << 'X';
//             } else {
//                 std::cout << '-';
//             }
//         }
//         std::cout << '\n';
//     }
//     std::cout << "\n\n";
// }

// void PrintBinary(int move) {
//     for (int i = 27; i >= 0; i--) {
//         if ((1 << i) & move) std::cout << '1';
//         else std::cout << '0';
//         if (i != 28 && i % 4 == 0) std::cout << ' ';
//     }
//     std::cout << '\n';
// }

// int main(int argc, char *argv[]) {
//     AllInit();

//     // int num = 2, nuts = 4;
//     // ASSERT(num == nuts);

//     //Part 7
//     // for (int index = 0; index < BRD_SQ_NUM; index++) {
//     //     if (index % 10 == 0)
//     //         std::cout << '\n';
//     //     printf("%5d", Sq120ToSq64[index]);
//     // }

//     // std::cout << '\n';
//     // std::cout << '\n';

//     // for (int index = 0; index < 64; ++index) {
//     //     if (index % 8 == 0)
//     //         std::cout << '\n';
//     //     printf("%5d", Sq64ToSq120[index]);
//     // }

//     //Part 8

//     // U64 playBitBoard = 0ULL;

//     // cout << "Start:\n";
//     // PrintBitBoard(playBitBoard);

//     // playBitBoard |= (1ULL << SQ64(D2));
//     // cout << "D2 added:\n";
//     // PrintBitBoard(playBitBoard);

//     // playBitBoard |= (1ULL << SQ64(B7));
//     // cout << "B7 added:\n";
//     // PrintBitBoard(playBitBoard);

//     // playBitBoard |= (1ULL << SQ64(G7));
//     // cout << "G7 added:\n";
//     // PrintBitBoard(playBitBoard);

//     // playBitBoard |= (1ULL << SQ64(F6));
//     // cout << "F6 added:\n";
//     // PrintBitBoard(playBitBoard);

//     // playBitBoard |= (1ULL << SQ64(H4));
//     // cout << "H4 added:\n";
//     // PrintBitBoard(playBitBoard);

//     //Part9
//     // U64 playBitBoard = 0ULL;

//     // playBitBoard |= (1ULL << SQ64(D2));
//     // playBitBoard |= (1ULL << SQ64(D3));
//     // playBitBoard |= (1ULL << SQ64(D4));

//     // cout << "D2: " << SQ64(D2) << '\n';
//     // PrintBitBoard(playBitBoard);

//     // int count =  CNT(playBitBoard);
//     // cout << "Count: " << count << '\n';

//     // int index = POP(&playBitBoard);
//     // cout << "Index: " << index << '\n';
//     // PrintBitBoard(playBitBoard);

//     // count = CNT(playBitBoard);
//     // cout << "Count: " << count << '\n';

//     // int sq64 = 0;
//     // while (playBitBoard) {
//     //     sq64 = POP(&playBitBoard);
//     //     cout << "Popped: " << sq64 << '\n';
//     //     PrintBitBoard(playBitBoard);
//     // }

//     //Part 10

//     // U64 playBitBoard = 0ULL;
//     // for (int i = 0; i < 64; ++i) {
//         // cout << "Index: " << i << '\n';
//         // PrintBitBoard(SetMask[i]);
//         // cout << '\n';

//     //     cout << "Index: " << i << '\n';
//     //     PrintBitBoard(ClearMask[i]);
//     //     cout << '\n';
//     // }

//     // SETBIT(&playBitBoard, 61);
//     // PrintBitBoard(playBitBoard);

//     // SETBIT(&playBitBoard, 11);
//     // PrintBitBoard(playBitBoard);

//     // CLRBIT(&playBitBoard, 61);
//     // PrintBitBoard(playBitBoard);

//     //Part 11
//     // int PieceOne = rand();
//     // int PieceTwo = rand();
//     // int PieceThree = rand();
//     // int PieceFour = rand();

//     // cout << hex << uppercase << PieceOne << '\n'; 
//     // cout << hex << uppercase << PieceTwo << '\n'; 
//     // cout << hex << uppercase << PieceThree << '\n'; 
//     // cout << hex << uppercase << PieceFour << '\n'; 

//     // int Key = PieceOne ^ PieceTwo ^ PieceFour;
//     // int TempKey = PieceTwo;
//     // TempKey ^= PieceThree;
//     // TempKey ^= PieceFour;
//     // TempKey ^= PieceOne;

//     // cout << "Key: " << hex << uppercase << Key << '\n';
//     // cout << "TempKey: " << hex << uppercase << TempKey << '\n';
//     // cout << '\n';

//     // TempKey ^= PieceThree;
//     // cout << "(Three out) TempKey: " << hex << uppercase << TempKey << '\n';

//     // TempKey ^= PieceThree;
//     // cout << "(Three in again) TempKey: " << hex << uppercase << TempKey << '\n';

//     //Part 12
//     // for (int i = 1; i <= 10; ++i)
//     //     cout << RAND_15 << ' ' << RAND_64 << '\n';

//     //Part 13
//     // S_BOARD board;
//     // ResetBoard(&board);

//     //Part 17->18
//     // S_BOARD board[1];

//     // ParseFen(START_FEN, board);
//     // PrintBoard(board);

//     // ParseFen(FEN1, board);
//     // PrintBoard(board);
    
//     // ParseFen(FEN2, board);
//     // PrintBoard(board);

//     // ParseFen(FEN3, board);
//     // PrintBoard(board);

//     // ParseFen(FEN4, board);
//     // PrintBoard(board);

//     // ParseFen(FEN5, board);
//     // PrintBoard(board);

//     // std::cout << WHITE << ' ' << BLACK << ' ' << BOTH << '\n';

//     // std::cout << "White - Black - BOTH big piece:\n";
//     // std::cout << board->bigPiece[WHITE] << '\n';
//     // std::cout << board->bigPiece[BLACK] << '\n';
//     // std::cout << std::dec << board->bigPiece[BOTH] << '\n';

//     // std::cout << "White - Black - BOTH major piece:\n";
//     // std::cout << board->majorPiece[WHITE] << '\n';
//     // std::cout << board->majorPiece[BLACK] << '\n';
//     // std::cout << std::dec << board->majorPiece[BOTH] << '\n';

//     // std::cout << "White - Black - BOTH minor piece:\n";
//     // std::cout << board->minorPiece[WHITE] << '\n';
//     // std::cout << board->minorPiece[BLACK] << '\n';
//     // std::cout << std::dec << board->minorPiece[BOTH] << '\n';

//     // std::cout << "White - Black king square:\n";
//     // std::cout << std::dec << board->kingSquare[WHITE] << '\n';
//     // std::cout << std::dec << board->kingSquare[BLACK] << '\n';

//     // std::cout << "castlePerm - enPas:\n";
//     // std::cout << std::dec << board->castlePerm << '\n';
//     // std::cout << std::dec << board->enPas << '\n';

//     //Part 20
//     // S_BOARD board[1];
//     // ParseFen(FEN6, board);
//     // PrintBoard(board);
//     // // board->posKey ^= SideKey;
//     // ASSERT(CheckBoard(board));

//     // std::cout << '\n';
//     // std::cout << "white pawns bitboard:\n";
//     // PrintBitBoard(board->pawnsBB[WHITE]);

//     // std::cout << '\n';
//     // std::cout << "black pawns bitboard:\n";
//     // PrintBitBoard(board->pawnsBB[BLACK]);

//     // std::cout << '\n';
//     // std::cout << "all pawns bitboard:\n";
//     // PrintBitBoard(board->pawnsBB[BOTH]);


//     // std::cout << '\n';
//     // std::cout << "white king bitboard:\n";
//     // PrintBitBoard(board->kingsBB[WHITE]);

//     // std::cout << '\n';
//     // std::cout << "black king bitboard:\n";
//     // PrintBitBoard(board->kingsBB[BLACK]);

//     // std::cout << '\n';
//     // std::cout << "white rooks bitboard:\n";
//     // PrintBitBoard(board->rooksBB[WHITE]);

//     // std::cout << '\n';
//     // std::cout << "black rooks bitboard:\n";
//     // PrintBitBoard(board->rooksBB[BLACK]);

//     // std::cout << '\n';
//     // std::cout << "white knights bitboard:\n";
//     // PrintBitBoard(board->knightsBB[WHITE]);

//     // std::cout << '\n';
//     // std::cout << "black knights bitboard:\n";
//     // PrintBitBoard(board->knightsBB[BLACK]);

//     // std::cout << '\n';
//     // std::cout << "white bishops bitboard:\n";
//     // PrintBitBoard(board->bishopsBB[WHITE]);

//     // std::cout << '\n';
//     // std::cout << "black bishops bitboard:\n";
//     // PrintBitBoard(board->bishopsBB[BLACK]);

//     // std::cout << '\n';
//     // std::cout << "all pieces white bitboard:\n";
//     // PrintBitBoard(board->allPiecesBB[WHITE]);

//     // std::cout << '\n';
//     // std::cout << "all pieces black bitboard:\n";
//     // PrintBitBoard(board->allPiecesBB[BLACK]);

//     // std::cout << '\n';
//     // std::cout << "all pieces both bitboard:\n";
//     // PrintBitBoard(board->allPiecesBB[BOTH]);

//     // std::cout << "white pawns piece: " << board->pieceNum[wP] << '\n';
//     // std::cout << "black pawns piece: " << board->pieceNum[bP] << '\n';

//     //Part 23 - turn off debug for illegal FEN
//     // S_BOARD board[1];
//     // ParseFen(FEN10, board);
//     // PrintBoard(board);
//     // // ASSERT(CheckBoard(board));

//     // std::cout << "\n\nWhite is attacking:\n";
//     // ShowSqAttackedBySide(WHITE, board);

//     // std::cout << "\n\nBlack is attacking:\n";
//     // ShowSqAttackedBySide(BLACK, board);

//     //Part 26
//     // S_BOARD board[1];
//     // ParseFen(FEN6, board);
//     // PrintBoard(board);
//     // ASSERT(CheckBoard(board));

//     // int from = 6; int to = 12;
//     // int captured = wR; int promoted = bR;

//     // int move = ((from) | (to << 7) | (captured << 14) | (promoted << 20));
//     // std::cout << '\n';
//     // std::cout << std::hex << move << '\n';
//     // PrintBinary(move);
//     // std::cout << std::dec << "from: " << FROMSQ(move) << " to: " << TOSQ(move) << " captured: " << CAPTURED(move) << " promoted: " << PROMOTED(move) << '\n';
    
//     // // move |= MOVEFLAG_PAWNSTART;
//     // std::cout << "is pawn start: " << ((move & MOVEFLAG_PAWNSTART) ? "YES" : "NO") << '\n';

//     //Part 27
//     // int move = 0;
//     // int from = F2, to = G1;
//     // int cap = wN; int prom = bB;

//     // move = ((from) | (to << 7) | (cap << 14) | (prom << 20));
//     // std::cout << std::dec << "from: " << FROMSQ(move) << " to: " << TOSQ(move) << " captured: " << CAPTURED(move) << " promoted: " << PROMOTED(move) << '\n';

//     // std::cout << "Algebraic from: " << PrintSquare(from) << '\n';
//     // std::cout << "Algebraic to: " << PrintSquare(to) << '\n';
//     // std::cout << "Algebraic move: " << PrintMove(move) << '\n';

//     //Part 30
//     // S_BOARD board[1];
//     // ParseFen(FEN_WHITE_PAWNMOVES, board);
//     // PrintBoard(board);

//     // S_MOVELIST list[1];
//     // GenerateAllMoves(board, list);
//     // PrintMoveList(list);

//     //Part 31
//     // S_BOARD board[1];
//     // ParseFen(FEN_BLACK_PAWNMOVES, board);
//     // PrintBoard(board);

//     // S_MOVELIST list[1];
//     // GenerateAllMoves(board, list);
//     // PrintMoveList(list);

//     //Part 32
//     // S_BOARD board[1];
//     // // ParseFen(FEN_WHITE_PAWNMOVES, board);
//     // ParseFen(FEN_BLACK_PAWNMOVES, board);
//     // PrintBoard(board);

//     // S_MOVELIST list[1];
//     // GenerateAllMoves(board, list);
//     // // PrintMoveList(list);

//     //Part 33
//     // S_BOARD board[1];
//     // S_MOVELIST list[1];

//     // ParseFen(FEN_WHITE_KNIGHTSKINGS, board);
//     // // ParseFen(FEN_BLACK_KNIGHTSKINGS, board);
//     // PrintBoard(board);

//     // GenerateAllMoves(board, list);
//     // // PrintMoveList(list);

//     //Part 34
//     // S_BOARD board[1];
//     // S_MOVELIST list[1];

//     // // ParseFen(FEN_WHITE_ROOKS, board);
//     // // ParseFen(FEN_BLACK_ROOKS, board);

//     // // ParseFen(FEN_WHITE_QUEENS, board);
//     // ParseFen(FEN_BLACK_QUEENS, board);

//     // // ParseFen(FEN_WHITE_BISHOPS, board);
//     // // ParseFen(FEN_BLACK_BISHOPS, board);

//     // PrintBoard(board);

//     // GenerateAllMoves(board, list);
//     // // PrintMoveList(list);

//     //Part 35
//     // S_BOARD board[1];
//     // S_MOVELIST list[1];

//     // // ParseFen(FEN_WHITE_CASTLE2, board);
//     // // ParseFen(FEN_BLACK_CASTLE2, board);

//     // // ParseFen(FEN_WHITE_CASTLE3, board);
//     // // ParseFen(FEN_BLACK_CASTLE3, board);

//     // // ParseFen(FEN_WHITE_CASTLE4, board);
//     // // ParseFen(FEN_BLACK_CASTLE4, board);

//     // ParseFen(FEN_WHITE_CASTLE5, board);
//     // // ParseFen(FEN_BLACK_CASTLE5, board);

//     // PrintBoard(board);

//     // GenerateAllMoves(board, list);

//     //Part 36
//     // S_BOARD board[1];
//     // S_MOVELIST list[1];

//     // ParseFen(FEN11, board);
//     // PrintBoard(board);

//     // GenerateAllMoves(board, list);
//     // PrintMoveList(list);

//     //Part 43
//     // S_BOARD board[1];
//     // S_MOVELIST list[1];

//     // ParseFen(START_FEN, board);
//     // PrintBoard(board);

//     // GenerateAllMoves(board, list);
//     // PrintMoveList(list);

//     // for (int moveNum = 0; moveNum < list->count; ++moveNum) {
//     //     int move = list->moves[moveNum].move;
        
//     //     if (!MakeMove(board, move)) {
//     //         continue;
//     //     }

//     //     std::cout << "\nMADE: " << PrintMove(move) << '\n';
//     //     PrintBoard(board);

//     //     TakeMove(board);
//     //     std::cout << "\nTAKEN: " << PrintMove(move) << '\n';
//     //     PrintBoard(board);

//     //     getchar();
//     // }

//     //Part 44
//     // S_BOARD board[1];
//     // S_MOVELIST list[1];

//     // // ParseFen(START_FEN, board);
//     // // ParseFen(FEN_PERFT_2, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_3, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_4, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_8, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_13, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_14, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_17, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_19, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_20, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_30, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_40, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_47, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_64, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_111, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_111, board);
//     // // PerftTest(5, board);

//     // // ParseFen(FEN_PERFT_120, board);
//     // // PerftTest(5, board);

//     // ParseFen(FEN_PERFT_126, board);
//     // PerftTest(5, board);

//     //Part 48
//     // S_BOARD board[1];
//     // S_MOVELIST list[1];

//     // ParseFen(FEN_PARSEMOVE_1, board);

//     // char input[6];
//     // int move = NOMOVE;

//     // while (true) {
//     //     PrintBoard(board);
//     //     std:: cout << "Please enter a move > ";
//     //     std::cin.getline(input, sizeof(input));

//     //     if (input[0] == 'q') {
//     //         break;
//     //     } else if (input[0] == 't') {
//     //         TakeMove(board);
//     //     } else {
//     //         move = ParseMove(input, board);
//     //         if (move != NOMOVE) {
//     //             MakeMove(board, move);
//     //         } else {
//     //             std::cout << "Illegal move\n";
//     //         }
//     //     }

//     //     if (std::cin.fail()) {
//     //         std::cout << "Please enter the move with correct length. Input was too long!\n";
//     //         std::cin.clear();
//     //     }
                
//     //     std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
//     // }

//     //Part 49
//     // S_BOARD board[1];
//     // S_MOVELIST list[1];

//     // ParseFen(START_FEN, board);

//     // char input[6];
//     // int move = NOMOVE;

//     // while (true) {
//     //     PrintBoard(board);
//     //     std:: cout << "Please enter a move > ";
//     //     std::cin.getline(input, sizeof(input));

//     //     if (input[0] == 'q') {
//     //         break;
//     //     } else if (input[0] == 't') {
//     //         TakeMove(board);
//     //     } else if (input[0] == 'p') {
//     //         PerftTest(4, board);
//     //     } else {
//     //         move = ParseMove(input, board);
//     //         if (move != NOMOVE) {
//     //             MakeMove(board, move);
//     //             if (IsRepetition(board)) {
//     //                 std::cout << "REP SEEN\n";
//     //             }
//     //         } else {
//     //             std::cout << "Illegal move\n";
//     //         }
//     //     }

//     //     if (std::cin.fail()) {
//     //         std::cout << "Please enter the move with correct length. Input was too long!\n";
//     //         std::cin.clear();
//     //     }
                
//     //     std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
//     // }

//     //Part 53
//     // S_BOARD board[1];
//     // S_MOVELIST list[1];

//     // ParseFen(START_FEN, board);

//     // char input[6];
//     // int move = NOMOVE;
//     // int Max = 0;

//     // while (true) {
//     //     PrintBoard(board);
//     //     std:: cout << "Please enter a move > ";
//     //     std::cin.getline(input, sizeof(input));

//     //     if (input[0] == 'q') {
//     //         break;
//     //     } else if (input[0] == 't') {
//     //         TakeMove(board);
//     //     } else if (input[0] == 'p') {
//     //         // PerftTest(4, board);
//     //         Max = GetPvLine(4, board);
//     //         std::cout << "PvLine of " << Max << " Moves: ";
//     //         for (int PvNum = 0; PvNum < Max; ++PvNum) {
//     //             move = board->PvArray[PvNum];
//     //             std::cout << " " << PrintMove(move);
//     //         }
//     //         std::cout << '\n';
//     //     } else {
//     //         move = ParseMove(input, board);
//     //         if (move != NOMOVE) {
//     //             StorePvMove(board, move);
//     //             MakeMove(board, move);
//     //             // if (IsRepetition(board)) {
//     //             //     std::cout << "REP SEEN\n";
//     //             // }
//     //         } else {
//     //             std::cout << "Illegal move\n";
//     //         }
//     //     }

//     //     if (std::cin.fail()) {
//     //         std::cout << "Please enter the move with correct length. Input was too long!\n";
//     //         std::cin.clear();
//     //     }
                
//     //     std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
//     // }

//     //Part 60
//     // S_BOARD board[1];
//     // S_MOVELIST list[1];
//     // S_SEARCHINFO info[1];

//     // // ParseFen(START_FEN, board);
//     // // ParseFen(FEN_WAC1, board);

//     // // ParseFen(FEN_MATEIN2_1, board);
//     // // ParseFen(FEN_MATEIN2_2, board);
//     // // ParseFen(FEN_MATEIN2_3, board);
//     // // ParseFen(FEN_MATEIN2_4, board);
//     // ParseFen(FEN_MATEIN2_5, board);

//     // char input[6];
//     // int move = NOMOVE;
//     // int Max = 0;

//     // while (true) {
//     //     PrintBoard(board);
//     //     std:: cout << "Please enter a move > ";
//     //     std::cin.getline(input, sizeof(input));

//     //     if (input[0] == 'q') {
//     //         break;
//     //     } else if (input[0] == 't') {
//     //         TakeMove(board);
//     //     } else if (input[0] == 's') {
//     //         info->depth = 4;
//     //         SearchPosition(board, info);
//     //     } else {
//     //         move = ParseMove(input, board);
//     //         if (move != NOMOVE) {
//     //             StorePvMove(board, move);
//     //             MakeMove(board, move);
//     //             // if (IsRepetition(board)) {
//     //             //     std::cout << "REP SEEN\n";
//     //             // }
//     //         } else {
//     //             std::cout << "Illegal move\n";
//     //         }
//     //     }

//     //     if (std::cin.fail()) {
//     //         std::cout << "Please enter the move with correct length. Input was too long!\n";
//     //         std::cin.clear();
//     //     }
                
//     //     std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
//     // }

//     //Part 61
//     // S_BOARD board[1];
//     // // InitPvTable(board->PvTable); -> move to S_BOARD constructor

//     // S_MOVELIST list[1];
//     // S_SEARCHINFO info[1];

//     // // ParseFen(FEN_WAC1, board);
//     // ParseFen(FEN_WAC2, board);
    
//     // // ParseFen(FEN_MATEIN3_1, board);
//     // // ParseFen(FEN_MATEIN3_2, board);
    
//     // // ParseFen(FEN_MATEIN5_1, board);
//     // // ParseFen(FEN_MATEIN5_2, board);

//     // char input[6];
//     // int move = NOMOVE;
//     // int Max = 0;

//     // while (true) {
//     //     PrintBoard(board);
//     //     std:: cout << "Please enter a move > ";
//     //     std::cin.getline(input, sizeof(input));

//     //     if (input[0] == 'q') {
//     //         break;
//     //     } else if (input[0] == 't') {
//     //         TakeMove(board);
//     //     } else if (input[0] == 's') {
//     //         info->depth = 6;
//     //         SearchPosition(board, info);
//     //     } else {
//     //         move = ParseMove(input, board);
//     //         if (move != NOMOVE) {
//     //             StorePvMove(board, move);
//     //             MakeMove(board, move);
//     //             // if (IsRepetition(board)) {
//     //             //     std::cout << "REP SEEN\n";
//     //             // }
//     //         } else {
//     //             std::cout << "Illegal move\n";
//     //         }
//     //     }

//     //     if (std::cin.fail()) {
//     //         std::cout << "Please enter the move with correct length. Input was too long!\n";
//     //         std::cin.clear();
//     //     }
                
//     //     std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
//     // }

//     //Part 64
//     // S_BOARD board[1];
//     // // InitPvTable(board->PvTable); -> move to S_BOARD constructor

//     // S_MOVELIST list[1];
//     // S_SEARCHINFO info[1];

//     // ParseFen(FEN_WAC2, board);

//     // char input[6];
//     // int move = NOMOVE;
//     // int Max = 0;

//     // while (true) {
//     //     PrintBoard(board);
//     //     std:: cout << "Please enter a move > ";
//     //     std::cin.getline(input, sizeof(input));

//     //     if (input[0] == 'q') {
//     //         break;
//     //     } else if (input[0] == 't') {
//     //         TakeMove(board);
//     //     } else if (input[0] == 's') {
//     //         info->depth = 6;
//     //         SearchPosition(board, info);
//     //     } else {
//     //         move = ParseMove(input, board);
//     //         if (move != NOMOVE) {
//     //             StorePvMove(board, move);
//     //             MakeMove(board, move);
//     //             // if (IsRepetition(board)) {
//     //             //     std::cout << "REP SEEN\n";
//     //             // }
//     //         } else {
//     //             std::cout << "Illegal move\n";
//     //         }
//     //     }

//     //     if (std::cin.fail()) {
//     //         std::cout << "Please enter the move with correct length. Input was too long!\n";
//     //         std::cin.clear();
//     //     }
                
//     //     std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
//     // }

//     //Self-test
//     // S_BOARD board[1];

//     // S_MOVELIST list[1];
//     // S_SEARCHINFO info[1];

//     // ParseFen(START_FEN, board);

//     // char input[6];
//     // int move = NOMOVE;
//     // int Max = 0;

//     // int timePerMove = 10000;

//     // while (true) {
//     //     PrintBoard(board);
//     //     std:: cout << "Please enter a move > ";
//     //     std::cin.getline(input, sizeof(input));

//     //     if (input[0] == 'q') {
//     //         break;
//     //     } else if (input[0] == 't') {
//     //         TakeMove(board);
//     //     } else if (input[0] == 's') {
//     //         // info->depth = 10;
//     //         // info->depthSet = TRUE;
//     //         info->timeSet = TRUE;
//     //         info->startTime = GetTimeMs();
//     //         info->stopTime = info->startTime + timePerMove;
//     //         info->depth = MAXDEPTH;
//     //         SearchPosition(board, info);
//     //     } else {
//     //         move = ParseMove(input, board);
//     //         if (move != NOMOVE) {
//     //             StorePvMove(board, move);
//     //             if (!MakeMove(board, move)) {
//     //                 std::cout << "Illegal move\n";
//     //             } else {
//     //                 std::cout << "Engine make move: " << PrintMove(move) << '\n';
//     //             }
//     //         } else {
//     //             std::cout << "Illegal move\n";
//     //         }
//     //     }

//     //     if (std::cin.fail()) {
//     //         std::cout << "Please enter the move with correct length. Input was too long!\n";
//     //         std::cin.clear();
//     //         std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
//     //     }
//     // }

//     // Uci_Loop();

//     //Part 73
//     S_BOARD board[1];
//     S_SEARCHINFO info[1];
//     info->quit = FALSE;
//     InitHashTable(HashTable, 64);

//     setbuf(stdin, NULL);
//     setbuf(stdout, NULL);
    
//     for (int ArgNum = 0; ArgNum < argc; ++ArgNum) {
//     	if (strncmp(argv[ArgNum], "NoBook", 6) == 0) {
//     		EngineOptions->UseBook = FALSE;
//     		std::cout << "Book Off\n";
//     	}
//     }

//     std::cout << "Welcome to Unstoppable Evaluation Tool (UET)!\n";

// 	char line[256];
// 	while (TRUE) {
// 		if (!std::cin.getline(line, sizeof(line))) {
// 			std::cin.clear();
// 			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
// 			continue;
// 		}

// 		if (!strncmp(line, "uci", 3)) {
// 			Uci_Loop(board, info);
// 			if(info->quit == TRUE) break;
// 			continue;

// 		} else if (!strncmp(line, "quit", 4)) {
// 			break;
// 		}
// 	}

//     CleanPolyBook();
//     delete[] HashTable->pTable;

//     return 0;
// }