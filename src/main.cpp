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

// main.cpp
#include "types.h"
#include "evaluation_types.h"
#include "attack_gen.h"
#include "board.h"
#include "eval_info.h"
#include "bitboards.h"

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdint> // Cho uint64_t nếu dùng trực tiếp

// Định nghĩa giá trị quân cờ mẫu (nên dùng giá trị thực tế của bạn)
// Giá trị cho NO_PIECE, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK
// Đảm bảo thứ tự khớp với enum Piece trong types.h
const int pieceMaterial[] = { 0, 100, 300, 310, 500, 900, 10000, 100, 300, 310, 500, 900, 10000 };

int main() {
    AllInit(); // Quan trọng: Gọi AllInit trước khi làm việc với board
    std::cout << "--- Kiem tra EvalInfo ---" << std::endl;
    S_Board board;
    std::memset(&board, 0, sizeof(S_Board)); // Xóa sạch board

    // --- Thiết lập cơ bản ---
    for(int i=0; i<BRD_SQ_NUM; ++i) board.pieces[i] = OFFBOARD;
    for(int i=0; i<64; ++i) board.pieces[Sq64ToSq120[i]] = NO_PIECE;

    board.pieces[E1] = wK; board.pieces[A1] = wR; board.pieces[E8] = bK; board.pieces[D7] = bP;
    board.KingSq[WHITE] = E1; board.KingSq[BLACK] = E8;
    board.kingsBB[WHITE] |= SF::square_bb(SF::SQ_E1);
    board.rooksBB[WHITE] |= SF::square_bb(SF::SQ_A1);
    board.kingsBB[BLACK] |= SF::square_bb(SF::SQ_E8);
    board.pawnsBB[BLACK] |= SF::square_bb(SF::SQ_D7);
    board.allPiecesBB[WHITE] = board.kingsBB[WHITE] | board.rooksBB[WHITE];
    board.allPiecesBB[BLACK] = board.kingsBB[BLACK] | board.pawnsBB[BLACK];
    board.allPiecesBB[BOTH] = board.allPiecesBB[WHITE] | board.allPiecesBB[BLACK];
    board.side = WHITE; board.enPas = NO_SQ; board.castlePerm = 0;

    // --- *** BỔ SUNG KHỞI TẠO ĐẦY ĐỦ *** ---

    // e.1) Khởi tạo pceNum, pList, material
    // Reset pceNum, pList, material về giá trị mặc định/trống
    for(int i=0; i<PIECE_NB; ++i) board.pceNum[i] = 0;
    for(int pc=0; pc<PIECE_NB; ++pc) for(int p=0; p<10; ++p) board.pList[pc][p] = NO_SQ; // NO_SQ từ types.h cho ô trống
    board.material[WHITE] = 0; board.material[BLACK] = 0;

    // Cập nhật cho từng quân dựa trên mảng pieces
    int pIndex;
    for (int sq120 = 0; sq120 < BRD_SQ_NUM; ++sq120) {
        int piece = board.pieces[sq120];
        if (piece != NO_PIECE && piece != OFFBOARD) {
            SF::Color color = (piece >= wP && piece <= wK) ? SF::WHITE : SF::BLACK;
            pIndex = board.pceNum[piece]++; // Lấy index và tăng số lượng
            board.pList[piece][pIndex] = sq120; // Lưu vị trí 120
            board.material[color] += pieceMaterial[piece]; // Cộng vật chất
        }
    }

    // e.2) Cập nhật bigPce, majPce, minPce
    // Reset trước
    board.bigPce[WHITE] = 0; board.bigPce[BLACK] = 0; board.bigPce[BOTH] = 0;
    board.majPce[WHITE] = 0; board.majPce[BLACK] = 0; board.majPce[BOTH] = 0;
    board.minPce[WHITE] = 0; board.minPce[BLACK] = 0; board.minPce[BOTH] = 0;
    // Tính toán dựa trên pceNum
    for(int pce = wP; pce <= bK; ++pce) { // Dùng enum Piece từ types.h
        if(board.pceNum[pce] > 0) {
            SF::Color color = (pce >= wP && pce <= wK) ? SF::WHITE : SF::BLACK;
            if (pce != wP && pce != bP) { // Không phải Tốt
                board.bigPce[color] += board.pceNum[pce];
                if (pce == wR || pce == wQ || pce == bR || pce == bQ) {
                    board.majPce[color] += board.pceNum[pce];
                } else if (pce == wN || pce == wB || pce == bN || pce == bB) {
                    board.minPce[color] += board.pceNum[pce];
                }
                // Lưu ý: Vua cũng được tính là bigPce trong cách tính này
            }
        }
    }
    board.bigPce[BOTH] = board.bigPce[WHITE] + board.bigPce[BLACK];
    board.majPce[BOTH] = board.majPce[WHITE] + board.majPce[BLACK];
    board.minPce[BOTH] = board.minPce[WHITE] + board.minPce[BLACK];

    // e.3) Cập nhật ply, hisPly, posKey
    board.ply = 0;
    board.hisPly = 0;
    // board.posKey = 0; // Hoặc tính khóa Zobrist nếu có hàm

    // --- Hết phần thiết lập S_Board ---

    // 3. Tạo đối tượng EvalInfo::Info
    EvalInfo::Info ei(&board);

    // 4. In thông tin kiểm tra (Bỏ bớt debug chi tiết nếu muốn)
    std::cout << "Board Occupied (White):" << std::endl;
    PrintBitBoard(ei.pieces(SF::WHITE));
    std::cout << "Board Occupied (Black):" << std::endl;
    PrintBitBoard(ei.pieces(SF::BLACK));

    std::cout << "\nAttacked by White (All):" << std::endl;
    PrintBitBoard(ei.attackedBy[SF::WHITE][SF::ALL_PIECES]);
    std::cout << "Attacked by Black (All):" << std::endl;
    PrintBitBoard(ei.attackedBy[SF::BLACK][SF::ALL_PIECES]);

    std::cout << "\nAttacked by White Pawns:" << std::endl;
    PrintBitBoard(ei.attackedBy[SF::WHITE][SF::PAWN]);
     std::cout << "Attacked by Black Pawns:" << std::endl;
    PrintBitBoard(ei.attackedBy[SF::BLACK][SF::PAWN]);

    std::cout << "\nAttacked by White Rooks:" << std::endl;
    PrintBitBoard(ei.attackedBy[SF::WHITE][SF::ROOK]); // Kiểm tra giá trị này

    std::cout << "\nSquares attacked by >= 2 White pieces:" << std::endl;
    PrintBitBoard(ei.attackedBy2[SF::WHITE]); // Kiểm tra giá trị này
    std::cout << "Squares attacked by >= 2 Black pieces:" << std::endl;
    PrintBitBoard(ei.attackedBy2[SF::BLACK]);

    std::cout << "\nWhite King Ring:" << std::endl;
    PrintBitBoard(ei.kingRing[SF::WHITE]);
    std::cout << "Black King Ring:" << std::endl;
    PrintBitBoard(ei.kingRing[SF::BLACK]);

    std::cout << "\nWhite King Attackers Count: " << ei.kingAttackersCount[SF::BLACK] << std::endl;
    std::cout << "White King Attackers Weight: " << ei.kingAttackersWeight[SF::BLACK] << std::endl;
    std::cout << "Black King Attackers Count: " << ei.kingAttackersCount[SF::WHITE] << std::endl;
    std::cout << "Black King Attackers Weight: " << ei.kingAttackersWeight[SF::WHITE] << std::endl;

    std::cout << "\nWhite Mobility Area:" << std::endl;
    PrintBitBoard(ei.mobilityArea[SF::WHITE]);
     std::cout << "Black Mobility Area:" << std::endl;
    PrintBitBoard(ei.mobilityArea[SF::BLACK]);


    std::cout << "\n--- Hoan thanh kiem tra EvalInfo ---" << std::endl;


    return 0;
}