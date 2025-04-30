// #include "types.h"
// #include "evaluation_types.h"

// #include <iostream>

// int main() {
//     AllInit();

//     for (int index = 0; index < BRD_SQ_NUM; index++) {
//         if (index % 10 == 0)
//             std::cout << '\n';
//         printf("%5d", Sq120ToSq64[index]);
//     }

//     std::cout << '\n';
//     std::cout << '\n';

//     for (int index = 0; index < 64; ++index) {
//         if (index % 8 == 0)
//             std::cout << '\n';
//         printf("%5d", Sq64ToSq120[index]);
//     }

//     return 0;
// }


// #include "types.h"
// #include "evaluation_types.h" // Để dùng SF::Value, SF::Piece, SF::Square, mg_value, eg_value
// #include "attack_gen.h"       // Để dùng AttackGen::PseudoAttacks_KNIGHT/KING
//             // Để gọi AllInit() (đã có sẵn nếu main.cpp đã include)
// #include "board.h"            // Không cần trực tiếp cho test này, nhưng có thể cần nếu test sâu hơn
// #include <iostream>
// #include <iomanip>            // Để dùng std::setw cho output đẹp hơn

// // --- Khai báo hàm PrintBitBoard nếu chưa có trong header nào ---
// // (Nếu bạn có header cho bitboards.cpp thì include header đó thay vì khai báo ở đây)
// #include "bitboards.cpp"
// // --- Hoặc bạn có thể include "bitboards.cpp" trực tiếp, nhưng không khuyến khích ---


// int main() {
//     AllInit(); // Khởi tạo mọi thứ, bao gồm PSQT và Attack Tables

//     std::cout << "--- Kiem tra PSQT (Gia tri quan co MG/EG) ---" << std::endl;
//     // In giá trị của một vài quân cờ (lấy từ psqt.cpp)
//     // Lưu ý: PieceValue nằm trong namespace SF
//     SF::Piece testPieces[] = { SF::W_PAWN, SF::W_KNIGHT, SF::W_ROOK, SF::W_QUEEN };
//     for (SF::Piece pc : testPieces) {
//          // Giả sử PieceValue được khai báo toàn cục hoặc có thể truy cập được
//          // Nếu không, bạn cần sửa đổi psqt.cpp/h để truy cập được
//          // Ở đây ta giả định nó truy cập được qua SF::PieceValue
//          // Note: PieceValue được định nghĩa trong psqt.cpp, cần đảm bảo nó có thể truy cập từ main
//         // Truy cập trực tiếp có thể cần khai báo extern trong header hoặc cách khác.
//         // *** Cập nhật: PieceValue được khai báo trong psqt.cpp nhưng không extern.
//         // *** Để test dễ dàng, ta sẽ in giá trị mặc định từ evaluation_types.h
//         // *** Nếu muốn test giá trị *sau khi* init, cần sửa code để truy cập PieceValue.

//         // Thay vào đó, ta in giá trị gốc từ header để minh họa:
//          std::cout << "Quan co " << int(pc) << ": ";
//          switch(SF::type_of(pc)) {
//              case SF::PAWN:
//                  std::cout << "Pawn  MG=" << std::setw(4) << SF::PawnValueMg << " EG=" << std::setw(4) << SF::PawnValueEg << std::endl;
//                  break;
//              case SF::KNIGHT:
//                   std::cout << "Knight MG=" << std::setw(4) << SF::KnightValueMg << " EG=" << std::setw(4) << SF::KnightValueEg << std::endl;
//                  break;
//              case SF::ROOK:
//                   std::cout << "Rook   MG=" << std::setw(4) << SF::RookValueMg << " EG=" << std::setw(4) << SF::RookValueEg << std::endl;
//                  break;
//              case SF::QUEEN:
//                   std::cout << "Queen  MG=" << std::setw(4) << SF::QueenValueMg << " EG=" << std::setw(4) << SF::QueenValueEg << std::endl;
//                  break;
//              default:
//                  std::cout << "Khac" << std::endl;
//                  break;
//          }
//          // Nếu bạn sửa code để truy cập được SF::PieceValue[PHASE][PIECE] sau init:
//          // std::cout << "Quan co " << int(pc) << ": MG=" << std::setw(4) << SF::mg_value(SF::PieceValue[SF::MG][pc])
//          //           << " EG=" << std::setw(4) << SF::eg_value(SF::PieceValue[SF::EG][pc]) << std::endl;

//     }
//      std::cout << std::endl;


//     std::cout << "--- Kiem tra Attack Generation (Bang tinh san) ---" << std::endl;

//     // Chọn một ô để kiểm tra, ví dụ E4 (trong hệ 64 ô)
//     SF::Square testSq = SF::SQ_E4; // SQ_E4 = 28 trong enum Square của evaluation_types.h
//     std::cout << "Tan cong cua Ma (Knight) tu o E4 (" << testSq << "):" << std::endl;
//     PrintBitBoard(AttackGen::PseudoAttacks_KNIGHT[testSq]);

//     std::cout << "Tan cong cua Vua (King) tu o E4 (" << testSq << "):" << std::endl;
//     PrintBitBoard(AttackGen::PseudoAttacks_KING[testSq]);


//     // // --- In lại bảng map để tham khảo (tùy chọn) ---
//     // std::cout << "\n--- Bang Sq120ToSq64 ---" << std::endl;
//     // for (int index = 0; index < BRD_SQ_NUM; index++) {
//     //     if (index % 10 == 0)
//     //         std::cout << '\n';
//     //     printf("%5d", Sq120ToSq64[index]); // Cần #include <cstdio> nếu dùng printf
//     // }
//     // std::cout << "\n\n--- Bang Sq64ToSq120 ---" << std::endl;
//     // for (int index = 0; index < 64; ++index) {
//     //     if (index % 8 == 0)
//     //         std::cout << '\n';
//     //     printf("%5d", Sq64ToSq120[index]); // Cần #include <cstdio> nếu dùng printf
//     // }
//     // std::cout << std::endl;

//     return 0;
// }


// main.cpp

// // main.cpp
// #include "types.h"
// #include "evaluation_types.h"
// #include "attack_gen.h"
// #include "board.h"
// #include "eval_info.h"
// #include "bitboards.h"

// #include <iostream>
// #include <iomanip>
// #include <cstring>
// #include <cstdint> // Cho uint64_t nếu dùng trực tiếp

// // Định nghĩa giá trị quân cờ mẫu (nên dùng giá trị thực tế của bạn)
// // Giá trị cho NO_PIECE, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK
// // Đảm bảo thứ tự khớp với enum Piece trong types.h
// const int pieceMaterial[] = { 0, 100, 300, 310, 500, 900, 10000, 100, 300, 310, 500, 900, 10000 };

// int main() {
//     AllInit(); // Quan trọng: Gọi AllInit trước khi làm việc với board
//     std::cout << "--- Kiem tra EvalInfo ---" << std::endl;
//     S_Board board;
//     std::memset(&board, 0, sizeof(S_Board)); // Xóa sạch board

//     // --- Thiết lập cơ bản ---
//     for(int i=0; i<BRD_SQ_NUM; ++i) board.pieces[i] = OFFBOARD;
//     for(int i=0; i<64; ++i) board.pieces[Sq64ToSq120[i]] = NO_PIECE;

//     board.pieces[E1] = wK; board.pieces[A1] = wR; board.pieces[E8] = bK; board.pieces[D7] = bP;
//     board.KingSq[WHITE] = E1; board.KingSq[BLACK] = E8;
//     board.kingsBB[WHITE] |= SF::square_bb(SF::SQ_E1);
//     board.rooksBB[WHITE] |= SF::square_bb(SF::SQ_A1);
//     board.kingsBB[BLACK] |= SF::square_bb(SF::SQ_E8);
//     board.pawnsBB[BLACK] |= SF::square_bb(SF::SQ_D7);
//     board.allPiecesBB[WHITE] = board.kingsBB[WHITE] | board.rooksBB[WHITE];
//     board.allPiecesBB[BLACK] = board.kingsBB[BLACK] | board.pawnsBB[BLACK];
//     board.allPiecesBB[BOTH] = board.allPiecesBB[WHITE] | board.allPiecesBB[BLACK];
//     board.side = WHITE; board.enPas = NO_SQ; board.castlePerm = 0;

//     // --- *** BỔ SUNG KHỞI TẠO ĐẦY ĐỦ *** ---

//     // e.1) Khởi tạo pceNum, pList, material
//     // Reset pceNum, pList, material về giá trị mặc định/trống
//     for(int i=0; i<PIECE_NB; ++i) board.pceNum[i] = 0;
//     for(int pc=0; pc<PIECE_NB; ++pc) for(int p=0; p<10; ++p) board.pList[pc][p] = NO_SQ; // NO_SQ từ types.h cho ô trống
//     board.material[WHITE] = 0; board.material[BLACK] = 0;

//     // Cập nhật cho từng quân dựa trên mảng pieces
//     int pIndex;
//     for (int sq120 = 0; sq120 < BRD_SQ_NUM; ++sq120) {
//         int piece = board.pieces[sq120];
//         if (piece != NO_PIECE && piece != OFFBOARD) {
//             SF::Color color = (piece >= wP && piece <= wK) ? SF::WHITE : SF::BLACK;
//             pIndex = board.pceNum[piece]++; // Lấy index và tăng số lượng
//             board.pList[piece][pIndex] = sq120; // Lưu vị trí 120
//             board.material[color] += pieceMaterial[piece]; // Cộng vật chất
//         }
//     }

//     // e.2) Cập nhật bigPce, majPce, minPce
//     // Reset trước
//     board.bigPce[WHITE] = 0; board.bigPce[BLACK] = 0; board.bigPce[BOTH] = 0;
//     board.majPce[WHITE] = 0; board.majPce[BLACK] = 0; board.majPce[BOTH] = 0;
//     board.minPce[WHITE] = 0; board.minPce[BLACK] = 0; board.minPce[BOTH] = 0;
//     // Tính toán dựa trên pceNum
//     for(int pce = wP; pce <= bK; ++pce) { // Dùng enum Piece từ types.h
//         if(board.pceNum[pce] > 0) {
//             SF::Color color = (pce >= wP && pce <= wK) ? SF::WHITE : SF::BLACK;
//             if (pce != wP && pce != bP) { // Không phải Tốt
//                 board.bigPce[color] += board.pceNum[pce];
//                 if (pce == wR || pce == wQ || pce == bR || pce == bQ) {
//                     board.majPce[color] += board.pceNum[pce];
//                 } else if (pce == wN || pce == wB || pce == bN || pce == bB) {
//                     board.minPce[color] += board.pceNum[pce];
//                 }
//                 // Lưu ý: Vua cũng được tính là bigPce trong cách tính này
//             }
//         }
//     }
//     board.bigPce[BOTH] = board.bigPce[WHITE] + board.bigPce[BLACK];
//     board.majPce[BOTH] = board.majPce[WHITE] + board.majPce[BLACK];
//     board.minPce[BOTH] = board.minPce[WHITE] + board.minPce[BLACK];

//     // e.3) Cập nhật ply, hisPly, posKey
//     board.ply = 0;
//     board.hisPly = 0;
//     // board.posKey = 0; // Hoặc tính khóa Zobrist nếu có hàm

//     // --- Hết phần thiết lập S_Board ---

//     // 3. Tạo đối tượng EvalInfo::Info
//     EvalInfo::Info ei(&board);

//     // 4. In thông tin kiểm tra (Bỏ bớt debug chi tiết nếu muốn)
//     std::cout << "Board Occupied (White):" << std::endl;
//     PrintBitBoard(ei.pieces(SF::WHITE));
//     std::cout << "Board Occupied (Black):" << std::endl;
//     PrintBitBoard(ei.pieces(SF::BLACK));

//     std::cout << "\nAttacked by White (All):" << std::endl;
//     PrintBitBoard(ei.attackedBy[SF::WHITE][SF::ALL_PIECES]);
//     std::cout << "Attacked by Black (All):" << std::endl;
//     PrintBitBoard(ei.attackedBy[SF::BLACK][SF::ALL_PIECES]);

//     std::cout << "\nAttacked by White Pawns:" << std::endl;
//     PrintBitBoard(ei.attackedBy[SF::WHITE][SF::PAWN]);
//      std::cout << "Attacked by Black Pawns:" << std::endl;
//     PrintBitBoard(ei.attackedBy[SF::BLACK][SF::PAWN]);

//     std::cout << "\nAttacked by White Rooks:" << std::endl;
//     PrintBitBoard(ei.attackedBy[SF::WHITE][SF::ROOK]); // Kiểm tra giá trị này

//     std::cout << "\nSquares attacked by >= 2 White pieces:" << std::endl;
//     PrintBitBoard(ei.attackedBy2[SF::WHITE]); // Kiểm tra giá trị này
//     std::cout << "Squares attacked by >= 2 Black pieces:" << std::endl;
//     PrintBitBoard(ei.attackedBy2[SF::BLACK]);

//     std::cout << "\nWhite King Ring:" << std::endl;
//     PrintBitBoard(ei.kingRing[SF::WHITE]);
//     std::cout << "Black King Ring:" << std::endl;
//     PrintBitBoard(ei.kingRing[SF::BLACK]);

//     std::cout << "\nWhite King Attackers Count: " << ei.kingAttackersCount[SF::BLACK] << std::endl;
//     std::cout << "White King Attackers Weight: " << ei.kingAttackersWeight[SF::BLACK] << std::endl;
//     std::cout << "Black King Attackers Count: " << ei.kingAttackersCount[SF::WHITE] << std::endl;
//     std::cout << "Black King Attackers Weight: " << ei.kingAttackersWeight[SF::WHITE] << std::endl;

//     std::cout << "\nWhite Mobility Area:" << std::endl;
//     PrintBitBoard(ei.mobilityArea[SF::WHITE]);
//      std::cout << "Black Mobility Area:" << std::endl;
//     PrintBitBoard(ei.mobilityArea[SF::BLACK]);


//     std::cout << "\n--- Hoan thanh kiem tra EvalInfo ---" << std::endl;


//     return 0;
// }




// // main.cpp - Modified for Pawn Analysis Debugging
// #include "types.h"
// #include "evaluation_types.h"
// #include "attack_gen.h"
// #include "board.h"         // Needed for S_Board, calculate_initial_pawn_key
// #include "pawn_analysis.h" // Needed for probe_pawn_table
// #include "pawn_entry.h"    // Needed for PawnEntry definition
// #include "bitboards.h"     // Needed for PrintBitBoard
// #include "zobrist.h"       // Needed for Zobrist keys (used in calculate_initial_pawn_key)

// #include <iostream>
// #include <iomanip>
// #include <cstring> // For std::memset

// // Helper to set up a piece on the board (both array and bitboards)
// void SetupPiece(S_Board* pos, int piece, SF::Square sq64) {
//     int sq120 = SQ120(sq64);
//     if (sq120 == OFFBOARD || sq120 == NO_SQ) return; // Invalid square

//     pos->pieces[sq120] = piece;

//     SF::Color color = (piece >= wP && piece <= wK) ? SF::WHITE : SF::BLACK;
//     SF::PieceType pt = SF::type_of(static_cast<SF::Piece>(piece));
//     SF::Bitboard bb = SF::square_bb(sq64);

//     pos->allPiecesBB[color] |= bb;
//     pos->allPiecesBB[BOTH] |= bb;

//     switch (pt) {
//         case SF::PAWN:   pos->pawnsBB[color] |= bb; break;
//         // Add cases for other pieces if needed for a more complex setup
//         // case SF::KNIGHT: pos->knightsBB[color] |= bb; break;
//         // case SF::BISHOP: pos->bishopsBB[color] |= bb; break;
//         // case SF::ROOK:   pos->rooksBB[color] |= bb; break;
//         // case SF::QUEEN:  pos->queensBB[color] |= bb; break;
//         // case SF::KING:   pos->kingsBB[color] |= bb; pos->KingSq[color] = sq120; break;
//         default: break;
//     }
//     // NOTE: This simplified setup doesn't update pList, pceNum, material etc.
//     // For full correctness in a real evaluation, these would also need updating.
//     // However, for pawn analysis which primarily relies on pawnsBB and pawnKey,
//     // this might be sufficient for basic debugging.
// }

// int main() {
//     // 1. Initialize everything (including Zobrist keys, attack tables etc.)
//     AllInit();
//     std::cout << "--- Debugging Pawn Analysis ---" << std::endl;

//     // 2. Create and Clear Board Structure
//     S_Board board;
//     std::memset(&board, 0, sizeof(S_Board)); // Clear all members to 0/NULL/false

//     // Initialize board squares
//     for(int i=0; i<BRD_SQ_NUM; ++i) board.pieces[i] = OFFBOARD;
//     for(int i=0; i<64; ++i) board.pieces[SQ120(i)] = NO_PIECE;
//     board.side = WHITE; // Or BLACK, depending on whose turn you want to simulate
//     board.enPas = NO_SQ;
//     board.castlePerm = 0;
//     // Initialize other necessary fields if analyze_pawns indirectly depends on them
//     // (e.g., KingSq if king safety within pawn eval is considered, though unlikely)
//     // For now, assume pawn analysis only needs pawn positions.

//     // 3. Setup Pawn Structure for Testing
//     // Example: White pawns on e4, d4, f2. Black pawns on d5, e6, g7.
//     SetupPiece(&board, wP, SF::SQ_E4);
//     SetupPiece(&board, wP, SF::SQ_D4);
//     SetupPiece(&board, wP, SF::SQ_F2);
//     SetupPiece(&board, bP, SF::SQ_D5);
//     SetupPiece(&board, bP, SF::SQ_E6);
//     SetupPiece(&board, bP, SF::SQ_G7);
//     // Add more pawns or change positions as needed for your test case

//     std::cout << "\nBoard Pawn Setup (White):" << std::endl;
//     PrintBitBoard(board.pawnsBB[SF::WHITE]);
//     std::cout << "Board Pawn Setup (Black):" << std::endl;
//     PrintBitBoard(board.pawnsBB[SF::BLACK]);

//     // 4. Calculate the Zobrist Key for Pawns
//     board.pawnKey = calculate_initial_pawn_key(&board);
//     std::cout << "Calculated Pawn Key: 0x" << std::hex << board.pawnKey << std::dec << std::endl;

//     // 5. Probe the Pawn Hash Table
//     std::cout << "\nProbing Pawn Table..." << std::endl;
//     PawnEntry* pawnEntry = probe_pawn_table(&board);

//     // 6. Print the results from PawnEntry
//     if (pawnEntry) {
//         std::cout << "Pawn Entry Found/Calculated. Key in Entry: 0x" << std::hex << pawnEntry->key << std::dec << std::endl;
//         if (pawnEntry->key == board.pawnKey) {
//              std::cout << "(Key Match: Cache Hit or First Calculation)" << std::endl;
//         } else {
//              std::cout << "(Key Mismatch: Possible Hash Collision or Error!)" << std::endl;
//         }

//         std::cout << "\nPawn Scores:" << std::endl;
//         std::cout << "  White: MG = " << std::setw(5) << SF::mg_value(pawnEntry->scores[SF::WHITE])
//                   << " EG = " << std::setw(5) << SF::eg_value(pawnEntry->scores[SF::WHITE]) << std::endl;
//         std::cout << "  Black: MG = " << std::setw(5) << SF::mg_value(pawnEntry->scores[SF::BLACK])
//                   << " EG = " << std::setw(5) << SF::eg_value(pawnEntry->scores[SF::BLACK]) << std::endl;
//         std::cout << "  Total: MG = " << std::setw(5) << SF::mg_value(pawnEntry->scores[SF::WHITE] - pawnEntry->scores[SF::BLACK])
//                   << " EG = " << std::setw(5) << SF::eg_value(pawnEntry->scores[SF::WHITE] - pawnEntry->scores[SF::BLACK]) << std::endl;


//         std::cout << "\nPassed Pawns (White):" << std::endl;
//         PrintBitBoard(pawnEntry->passedPawns[SF::WHITE]);
//         std::cout << "Passed Pawns (Black):" << std::endl;
//         PrintBitBoard(pawnEntry->passedPawns[SF::BLACK]);

//         std::cout << "\nPawn Attacks (White):" << std::endl;
//         PrintBitBoard(pawnEntry->pawnAttacks[SF::WHITE]);
//         std::cout << "Pawn Attacks (Black):" << std::endl;
//         PrintBitBoard(pawnEntry->pawnAttacks[SF::BLACK]);

//         std::cout << "\nPawn Attack Span (White):" << std::endl;
//         PrintBitBoard(pawnEntry->pawnAttacksSpan[SF::WHITE]);
//         std::cout << "Pawn Attack Span (Black):" << std::endl;
//         PrintBitBoard(pawnEntry->pawnAttacksSpan[SF::BLACK]);

//     } else {
//         std::cerr << "Error: probe_pawn_table returned nullptr!" << std::endl;
//     }

//     std::cout << "\n--- Pawn Analysis Debugging Finished ---" << std::endl;

//     return 0;
// }








// main.cpp - Modified for Material Analysis Debugging
#include "types.h"             // Enum Piece (wP, bK), S_Board, BRD_SQ_NUM, etc.
#include "evaluation_types.h"  // SF:: types, Score, Phase, Value, etc.
#include "board.h"             // S_Board definition
#include "material_analysis.h" // probe_material_table, MaterialEntry
#include "material_entry.h"    // MaterialEntry definition

#include <iostream>
#include <iomanip>
#include <cstring>            // Cho std::memset
#include <cassert>            // Cho assert

// --- Helper Function to setup board and piece counts ---
// Quan trọng: Hàm này cần cập nhật cả pceNum và material
void SetupBoardForMaterialTest(S_Board* pos, const std::vector<std::pair<int, SF::Square>>& pieces) {
    // 1. Clear board state completely
    std::memset(pos, 0, sizeof(S_Board));
    for (int i = 0; i < BRD_SQ_NUM; ++i) pos->pieces[i] = OFFBOARD;
    for (int i = 0; i < 64; ++i) pos->pieces[SQ120(i)] = NO_PIECE;
    for (int i = 0; i < 13; ++i) pos->pceNum[i] = 0; // Rất quan trọng
    pos->material[WHITE] = 0;
    pos->material[BLACK] = 0;
    pos->side = WHITE; // Mặc định
    pos->enPas = NO_SQ;
    // Các giá trị khác có thể để mặc định (0) vì material analysis không dùng

    // Giá trị vật chất tham khảo (chỉ dùng để cập nhật pos->material)
    // Lấy từ SF:: nhưng dùng index của Vice Piece enum
    const int pieceMaterialValue[13] = {
        0, SF::PawnValueMg, SF::KnightValueMg, SF::BishopValueMg, SF::RookValueMg, SF::QueenValueMg, 0 /*King*/,
           SF::PawnValueMg, SF::KnightValueMg, SF::BishopValueMg, SF::RookValueMg, SF::QueenValueMg, 0 /*King*/
    };

    // 2. Place pieces and update counts/material
    for (const auto& p : pieces) {
        int vicePiece = p.first;
        SF::Square sq64 = p.second;
        int sq120 = SQ120(sq64);

        if (sq120 != OFFBOARD && sq120 != NO_SQ && vicePiece >= wP && vicePiece <= bK) {
            pos->pieces[sq120] = vicePiece;
            pos->pceNum[vicePiece]++; // Cập nhật số lượng

            SF::Color color = (vicePiece >= wP && vicePiece <= wK) ? SF::WHITE : SF::BLACK;
            pos->material[color] += pieceMaterialValue[vicePiece]; // Cập nhật vật chất (chỉ để tham khảo)

            // Cập nhật bitboards nếu cần cho non_pawn_material (nếu nó dùng BB thay vì pceNum)
            // Hiện tại non_pawn_material dùng pceNum nên không cần cập nhật BB ở đây
        }
    }
}

int main() {
    // 1. Initialize Engine Subsystems
    AllInit(); // Rất quan trọng! Gọi một lần duy nhất.
    std::cout << "--- Debugging Material Analysis ---" << std::endl;

    // 2. Create Board Structure
    S_Board board;

    // 3. Setup Material Configuration for Testing
    // Ví dụ 1: KRN vs KR (Trắng mạnh hơn)
    std::vector<std::pair<int, SF::Square>> setup1 = {
        {wK, SF::SQ_E1}, {wR, SF::SQ_A1}, {wN, SF::SQ_G1},
        {bK, SF::SQ_E8}, {bR, SF::SQ_H8}
    };
    std::cout << "\n--- Test Case 1: KRN vs KR ---" << std::endl;
    SetupBoardForMaterialTest(&board, setup1);

    // --- Print piece counts for verification ---
    std::cout << "Piece Counts (pceNum):" << std::endl;
    std::cout << "  wP:" << board.pceNum[wP] << " wN:" << board.pceNum[wN] << " wB:" << board.pceNum[wB]
              << " wR:" << board.pceNum[wR] << " wQ:" << board.pceNum[wQ] << " wK:" << board.pceNum[wK] << std::endl;
    std::cout << "  bP:" << board.pceNum[bP] << " bN:" << board.pceNum[bN] << " bB:" << board.pceNum[bB]
              << " bR:" << board.pceNum[bR] << " bQ:" << board.pceNum[bQ] << " bK:" << board.pceNum[bK] << std::endl;
    // --- End print piece counts ---

    // 4. Probe the Material Hash Table
    std::cout << "Probing Material Table..." << std::endl;
    MaterialEntry* entry1 = probe_material_table(&board); // Hàm này sẽ tự tính key

    // 5. Print the results from MaterialEntry
    if (entry1) {
        SF::Key currentKey = calculate_material_key(&board); // Tính lại key để so sánh
        std::cout << "Material Entry Found/Calculated." << std::endl;
        std::cout << "  Calculated Key: 0x" << std::hex << currentKey << std::dec << std::endl;
        std::cout << "  Key in Entry:   0x" << std::hex << entry1->key << std::dec << std::endl;
        assert(entry1->key == currentKey && "Material key mismatch!"); // Kiểm tra key khớp

        std::cout << "  Game Phase:     " << static_cast<int>(entry1->gamePhase)
                  << " (0=EG, 128=MG)"<< std::endl;

        std::cout << "  Imbalance Val:  " << std::setw(5) << entry1->imbalance_value << std::endl;
        SF::Score imbalanceScore = entry1->imbalance();
        std::cout << "  Imbalance Score: MG=" << std::setw(5) << SF::mg_value(imbalanceScore)
                  << " EG=" << std::setw(5) << SF::eg_value(imbalanceScore) << std::endl;

        std::cout << "  Default Factors: White=" << static_cast<int>(entry1->factor[SF::WHITE])
                  << " Black=" << static_cast<int>(entry1->factor[SF::BLACK])
                  << " (64=Normal, 0=Draw)" << std::endl;
        // Test scale_factor function (hiện tại trả về normal)
        std::cout << "  ScaleFactor(W): " << static_cast<int>(entry1->scale_factor(SF::WHITE)) << std::endl;
        std::cout << "  ScaleFactor(B): " << static_cast<int>(entry1->scale_factor(SF::BLACK)) << std::endl;

    } else {
        std::cerr << "Error: probe_material_table returned nullptr for Test Case 1!" << std::endl;
    }

    // Ví dụ 2: KB vs K (Trắng không có Tốt, nên factor[WHITE] có thể bị scale)
    std::vector<std::pair<int, SF::Square>> setup2 = {
        {wK, SF::SQ_E1}, {wB, SF::SQ_C1},
        {bK, SF::SQ_E8}
    };
    std::cout << "\n--- Test Case 2: KB vs K ---" << std::endl;
    SetupBoardForMaterialTest(&board, setup2);

    // --- Print piece counts for verification ---
    std::cout << "Piece Counts (pceNum):" << std::endl;
    std::cout << "  wP:" << board.pceNum[wP] << " wN:" << board.pceNum[wN] << " wB:" << board.pceNum[wB]
              << " wR:" << board.pceNum[wR] << " wQ:" << board.pceNum[wQ] << " wK:" << board.pceNum[wK] << std::endl;
    std::cout << "  bP:" << board.pceNum[bP] << " bN:" << board.pceNum[bN] << " bB:" << board.pceNum[bB]
              << " bR:" << board.pceNum[bR] << " bQ:" << board.pceNum[bQ] << " bK:" << board.pceNum[bK] << std::endl;
    // --- End print piece counts ---

    std::cout << "Probing Material Table..." << std::endl;
    MaterialEntry* entry2 = probe_material_table(&board);

    if (entry2) {
         SF::Key currentKey = calculate_material_key(&board);
         std::cout << "Material Entry Found/Calculated." << std::endl;
         std::cout << "  Calculated Key: 0x" << std::hex << currentKey << std::dec << std::endl;
         std::cout << "  Key in Entry:   0x" << std::hex << entry2->key << std::dec << std::endl;
         assert(entry2->key == currentKey && "Material key mismatch!");

         std::cout << "  Game Phase:     " << static_cast<int>(entry2->gamePhase) << std::endl;
         std::cout << "  Imbalance Val:  " << std::setw(5) << entry2->imbalance_value << std::endl;
         SF::Score imbalanceScore = entry2->imbalance();
         std::cout << "  Imbalance Score: MG=" << std::setw(5) << SF::mg_value(imbalanceScore)
                   << " EG=" << std::setw(5) << SF::eg_value(imbalanceScore) << std::endl;
         // Chú ý giá trị factor[WHITE] ở đây!
         std::cout << "  Default Factors: White=" << static_cast<int>(entry2->factor[SF::WHITE])
                   << " Black=" << static_cast<int>(entry2->factor[SF::BLACK]) << std::endl;

    } else {
         std::cerr << "Error: probe_material_table returned nullptr for Test Case 2!" << std::endl;
    }


     // Ví dụ 3: KQ vs KPPPP (Kiểm tra imbalance và phase)
     std::vector<std::pair<int, SF::Square>> setup3 = {
        {wK, SF::SQ_E1}, {wQ, SF::SQ_D1},
        {bK, SF::SQ_E8}, {bP, SF::SQ_A7}, {bP, SF::SQ_B7}, {bP, SF::SQ_C7}, {bP, SF::SQ_D7}
     };
    std::cout << "\n--- Test Case 3: KQ vs KPPPP ---" << std::endl;
    SetupBoardForMaterialTest(&board, setup3);

     // --- Print piece counts for verification ---
     std::cout << "Piece Counts (pceNum):" << std::endl;
     std::cout << "  wP:" << board.pceNum[wP] << " wN:" << board.pceNum[wN] << " wB:" << board.pceNum[wB]
               << " wR:" << board.pceNum[wR] << " wQ:" << board.pceNum[wQ] << " wK:" << board.pceNum[wK] << std::endl;
     std::cout << "  bP:" << board.pceNum[bP] << " bN:" << board.pceNum[bN] << " bB:" << board.pceNum[bB]
               << " bR:" << board.pceNum[bR] << " bQ:" << board.pceNum[bQ] << " bK:" << board.pceNum[bK] << std::endl;
     // --- End print piece counts ---

     std::cout << "Probing Material Table..." << std::endl;
     MaterialEntry* entry3 = probe_material_table(&board);

     if (entry3) {
         SF::Key currentKey = calculate_material_key(&board);
         std::cout << "Material Entry Found/Calculated." << std::endl;
         std::cout << "  Calculated Key: 0x" << std::hex << currentKey << std::dec << std::endl;
         std::cout << "  Key in Entry:   0x" << std::hex << entry3->key << std::dec << std::endl;
         assert(entry3->key == currentKey && "Material key mismatch!");

         std::cout << "  Game Phase:     " << static_cast<int>(entry3->gamePhase) << std::endl;
         std::cout << "  Imbalance Val:  " << std::setw(5) << entry3->imbalance_value << std::endl;
         SF::Score imbalanceScore = entry3->imbalance();
         std::cout << "  Imbalance Score: MG=" << std::setw(5) << SF::mg_value(imbalanceScore)
                   << " EG=" << std::setw(5) << SF::eg_value(imbalanceScore) << std::endl;
         std::cout << "  Default Factors: White=" << static_cast<int>(entry3->factor[SF::WHITE])
                   << " Black=" << static_cast<int>(entry3->factor[SF::BLACK]) << std::endl;
     } else {
         std::cerr << "Error: probe_material_table returned nullptr for Test Case 3!" << std::endl;
     }


    std::cout << "\n--- Material Analysis Debugging Finished ---" << std::endl;

    return 0;
}