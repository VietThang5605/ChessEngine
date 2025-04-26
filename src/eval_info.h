#ifndef EVAL_INFO_H_INCLUDED
#define EVAL_INFO_H_INCLUDED

#include "evaluation_types.h" // Chứa SF::Bitboard, SF::Color, SF::PieceType, etc.
#include "board.h"             // Chứa định nghĩa S_BOARD

// Forward declarations cho các cấu trúc sẽ được định nghĩa ở các bước sau
// Bạn có thể comment out các con trỏ này nếu chưa định nghĩa PawnEntry/MaterialEntry
struct PawnEntry;
struct MaterialEntry;

namespace EvalInfo {

    struct Info {
        // --- Con trỏ tới dữ liệu gốc ---
        const S_Board* pos_ptr;
        const PawnEntry* pawn_entry_ptr;    // Sẽ dùng ở bước sau
        const MaterialEntry* material_entry_ptr; // Sẽ dùng ở bước sau

        // --- Bản đồ tấn công ---
        SF::Bitboard attackedBy[SF::COLOR_NB][SF::PIECE_TYPE_NB];
        SF::Bitboard attackedBy2[SF::COLOR_NB]; // Ô bị >= 2 quân tấn công

        // --- Khu vực di chuyển & Vua ---
        SF::Bitboard mobilityArea[SF::COLOR_NB]; // Ô an toàn cho quân di chuyển
        SF::Bitboard kingRing[SF::COLOR_NB];     // Vùng quanh Vua
        int kingAttackersCount[SF::COLOR_NB];    // Số quân địch tấn công vùng Vua
        int kingAttackersWeight[SF::COLOR_NB];   // Trọng số quân địch tấn công vùng Vua
        // int kingAttacksCount[SF::COLOR_NB];    // (Tùy chọn) Số *lượt* tấn công vào ô cạnh Vua

        // --- Constructor: Tính toán tất cả thông tin ---
        Info(const S_Board* board_ptr, const PawnEntry* pe_ptr = nullptr, const MaterialEntry* me_ptr = nullptr);

        // Hàm helper để lấy quân cờ tại một ô (cần thiết vì S_BOARD dùng int)
        // Bạn có thể đặt hàm này ở một nơi phù hợp hơn nếu muốn
        inline SF::Piece piece_on(SF::Square s) const {
            // Giả sử pieces[] trong S_BOARD lưu trữ giá trị enum Piece hoặc tương đương
            return static_cast<SF::Piece>(pos_ptr->pieces[s]);
        }
         // Hàm helper lấy bitboard tất cả các quân
         inline SF::Bitboard pieces() const {
            return pos_ptr->allPiecesBB[SF::WHITE] | pos_ptr->allPiecesBB[SF::BLACK];
         }
         // Hàm helper lấy bitboard quân theo màu
         inline SF::Bitboard pieces(SF::Color c) const {
            return pos_ptr->allPiecesBB[c];
         }
          // Hàm helper lấy bitboard quân theo màu và loại
         inline SF::Bitboard pieces(SF::Color c, SF::PieceType pt) const {
              switch(pt) {
                 case SF::PAWN:   return pos_ptr->pawnsBB[c];
                 case SF::KNIGHT: return pos_ptr->knightsBB[c];
                 case SF::BISHOP: return pos_ptr->bishopsBB[c];
                 case SF::ROOK:   return pos_ptr->rooksBB[c];
                 case SF::QUEEN:  return pos_ptr->queensBB[c];
                 case SF::KING:   return pos_ptr->kingsBB[c];
                 default: return 0;
              }
         }
    };

} // namespace EvalInfo

#endif // EVAL_INFO_H_INCLUDED