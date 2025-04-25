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


#include "types.h"
#include "evaluation_types.h" // Để dùng SF::Value, SF::Piece, SF::Square, mg_value, eg_value
#include "attack_gen.h"       // Để dùng AttackGen::PseudoAttacks_KNIGHT/KING
            // Để gọi AllInit() (đã có sẵn nếu main.cpp đã include)
#include "board.h"            // Không cần trực tiếp cho test này, nhưng có thể cần nếu test sâu hơn
#include <iostream>
#include <iomanip>            // Để dùng std::setw cho output đẹp hơn

// --- Khai báo hàm PrintBitBoard nếu chưa có trong header nào ---
// (Nếu bạn có header cho bitboards.cpp thì include header đó thay vì khai báo ở đây)
#include "bitboards.cpp"
// --- Hoặc bạn có thể include "bitboards.cpp" trực tiếp, nhưng không khuyến khích ---


int main() {
    AllInit(); // Khởi tạo mọi thứ, bao gồm PSQT và Attack Tables

    std::cout << "--- Kiem tra PSQT (Gia tri quan co MG/EG) ---" << std::endl;
    // In giá trị của một vài quân cờ (lấy từ psqt.cpp)
    // Lưu ý: PieceValue nằm trong namespace SF
    SF::Piece testPieces[] = { SF::W_PAWN, SF::W_KNIGHT, SF::W_ROOK, SF::W_QUEEN };
    for (SF::Piece pc : testPieces) {
         // Giả sử PieceValue được khai báo toàn cục hoặc có thể truy cập được
         // Nếu không, bạn cần sửa đổi psqt.cpp/h để truy cập được
         // Ở đây ta giả định nó truy cập được qua SF::PieceValue
         // Note: PieceValue được định nghĩa trong psqt.cpp, cần đảm bảo nó có thể truy cập từ main
        // Truy cập trực tiếp có thể cần khai báo extern trong header hoặc cách khác.
        // *** Cập nhật: PieceValue được khai báo trong psqt.cpp nhưng không extern.
        // *** Để test dễ dàng, ta sẽ in giá trị mặc định từ evaluation_types.h
        // *** Nếu muốn test giá trị *sau khi* init, cần sửa code để truy cập PieceValue.

        // Thay vào đó, ta in giá trị gốc từ header để minh họa:
         std::cout << "Quan co " << int(pc) << ": ";
         switch(SF::type_of(pc)) {
             case SF::PAWN:
                 std::cout << "Pawn  MG=" << std::setw(4) << SF::PawnValueMg << " EG=" << std::setw(4) << SF::PawnValueEg << std::endl;
                 break;
             case SF::KNIGHT:
                  std::cout << "Knight MG=" << std::setw(4) << SF::KnightValueMg << " EG=" << std::setw(4) << SF::KnightValueEg << std::endl;
                 break;
             case SF::ROOK:
                  std::cout << "Rook   MG=" << std::setw(4) << SF::RookValueMg << " EG=" << std::setw(4) << SF::RookValueEg << std::endl;
                 break;
             case SF::QUEEN:
                  std::cout << "Queen  MG=" << std::setw(4) << SF::QueenValueMg << " EG=" << std::setw(4) << SF::QueenValueEg << std::endl;
                 break;
             default:
                 std::cout << "Khac" << std::endl;
                 break;
         }
         // Nếu bạn sửa code để truy cập được SF::PieceValue[PHASE][PIECE] sau init:
         // std::cout << "Quan co " << int(pc) << ": MG=" << std::setw(4) << SF::mg_value(SF::PieceValue[SF::MG][pc])
         //           << " EG=" << std::setw(4) << SF::eg_value(SF::PieceValue[SF::EG][pc]) << std::endl;

    }
     std::cout << std::endl;


    std::cout << "--- Kiem tra Attack Generation (Bang tinh san) ---" << std::endl;

    // Chọn một ô để kiểm tra, ví dụ E4 (trong hệ 64 ô)
    SF::Square testSq = SF::SQ_E4; // SQ_E4 = 28 trong enum Square của evaluation_types.h
    std::cout << "Tan cong cua Ma (Knight) tu o E4 (" << testSq << "):" << std::endl;
    PrintBitBoard(AttackGen::PseudoAttacks_KNIGHT[testSq]);

    std::cout << "Tan cong cua Vua (King) tu o E4 (" << testSq << "):" << std::endl;
    PrintBitBoard(AttackGen::PseudoAttacks_KING[testSq]);


    // // --- In lại bảng map để tham khảo (tùy chọn) ---
    // std::cout << "\n--- Bang Sq120ToSq64 ---" << std::endl;
    // for (int index = 0; index < BRD_SQ_NUM; index++) {
    //     if (index % 10 == 0)
    //         std::cout << '\n';
    //     printf("%5d", Sq120ToSq64[index]); // Cần #include <cstdio> nếu dùng printf
    // }
    // std::cout << "\n\n--- Bang Sq64ToSq120 ---" << std::endl;
    // for (int index = 0; index < 64; ++index) {
    //     if (index % 8 == 0)
    //         std::cout << '\n';
    //     printf("%5d", Sq64ToSq120[index]); // Cần #include <cstdio> nếu dùng printf
    // }
    // std::cout << std::endl;

    return 0;
}