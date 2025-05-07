#include "eval_info.h"
#include "attack_gen.h"       // Header chứa các hàm AttackGen::...
#include "evaluation_types.h" // Header chứa SF::...
#include "board.h"             // Header chứa S_BOARD, các hằng số quân cờ (wP, bK,...)
#include "types.h"

#include "init.h"
#include "bitboards.h"
#include "io.h"
// Include các header cần thiết khác nếu có
#include "pawn_entry.h"
#include "material_entry.h"

#include <cstring>   // Cho std::memset
#include <algorithm> // Cho std::max, std::min
#include <cmath>     // Cho std::abs


#include<iostream>
#include "bitboards.h"


namespace EvalInfo {

// --- KingAttackWeights (Lấy từ Stockfish 11 evaluate.cpp) ---
constexpr int KingAttackWeights[SF::PIECE_TYPE_NB] = { 0, 0, 81, 52, 44, 10 }; // P N B R Q K (Pawn=0, King=0)


// --- Constructor Implementation (Tối ưu hơn) ---
Info::Info(const S_BOARD* board_ptr, const PawnEntry* pe_ptr, const MaterialEntry* me_ptr)
 : pos_ptr(board_ptr), pawn_entry_ptr(pe_ptr), material_entry_ptr(me_ptr)
{

    // --- 1. Khởi tạo ---
    std::memset(attackedBy, 0, sizeof(attackedBy));
    std::memset(attackedBy2, 0, sizeof(attackedBy2));
    std::memset(mobilityArea, 0, sizeof(mobilityArea));
    std::memset(kingRing, 0, sizeof(kingRing));
    std::memset(kingAttackersCount, 0, sizeof(kingAttackersCount));
    std::memset(kingAttackersWeight, 0, sizeof(kingAttackersWeight));

        SF::Bitboard occupied = pieces(); // Lấy bitboard tất cả quân cờ

    // --- 2. Tính KingRing cho cả 2 bên trước ---
    for (SF::Color c : { SF::WHITE, SF::BLACK }) {
        int kingSq120 = pos_ptr->kingSquare[c]; // Lấy index 120-based từ S_BOARD

        // --- BẮT ĐẦU SỬA ---
        // Chuyển đổi sang 64-based trước khi ép kiểu sang SF::Square
        int kingSq64 = SQ64(kingSq120);

        // Kiểm tra xem ô 64-based có hợp lệ không
        if (kingSq64 >= 0 && kingSq64 < 64)
        {
            SF::Square ksq = static_cast<SF::Square>(kingSq64); // Ép kiểu index 64-based

            // Mảng PseudoAttacks_KING được truy cập bằng index 64-based (ksq)
            kingRing[c] = AttackGen::PseudoAttacks_KING[ksq] | SF::square_bb(ksq);

        }
        else
        {
            // Nếu ô vua 120-based không hợp lệ hoặc ngoài bàn cờ 64 ô
            kingRing[c] = 0; // Gán giá trị mặc định là 0
        }
    }

    // --- 3. Tính toán attackedBy và đồng thời cập nhật King Safety ---
    std::vector<std::string> piece_names = {"NO", "P", "N", "B", "R", "Q", "K"}; // Tên quân để debug

    for (SF::Color c : { SF::WHITE, SF::BLACK }) {
        SF::Color them = ~c;
        // --- Lặp qua từng loại quân ---
        for (SF::PieceType pt = SF::PAWN; pt <= SF::KING; ++pt) {
            SF::Bitboard piece_locations = pieces(c, pt); // Lấy bitboard vị trí quân

            SF::Bitboard current_attacks = 0; // Tấn công tổng hợp của loại quân này

            // --- Lặp qua từng quân cờ riêng lẻ của loại pt, màu c ---
            SF::Bitboard current_pieces = piece_locations;
            while (current_pieces) {
                SF::Square s = SF::pop_lsb(&current_pieces);
                SF::Bitboard attacks_from_s = 0; // Tấn công từ ô s này

                // Tính toán tấn công cho *chỉ quân cờ tại ô s* MỘT CÁCH CHÍNH XÁC
                if (pt == SF::PAWN) {
                    // Tính trực tiếp bằng shift:
                    if (c == SF::WHITE) attacks_from_s = SF::shift<SF::NORTH_WEST>(SF::square_bb(s)) | SF::shift<SF::NORTH_EAST>(SF::square_bb(s));
                    else attacks_from_s = SF::shift<SF::SOUTH_WEST>(SF::square_bb(s)) | SF::shift<SF::SOUTH_EAST>(SF::square_bb(s));


                }
                else if (pt == SF::KNIGHT) attacks_from_s = AttackGen::PseudoAttacks_KNIGHT[s];
                else if (pt == SF::KING) {
                    attacks_from_s = AttackGen::PseudoAttacks_KING[s];
                }  
                // Gọi hàm helper attacks_from_sliding đã được đưa ra ngoài
                else if (pt == SF::BISHOP) attacks_from_s = AttackGen::attacks_from_sliding(SF::square_bb(s), occupied, AttackGen::BishopDirections, 4);
                else if (pt == SF::ROOK) {
                    attacks_from_s = AttackGen::attacks_from_sliding(SF::square_bb(s), occupied, AttackGen::RookDirections, 4);

                }  
                else if (pt == SF::QUEEN)  attacks_from_s = AttackGen::attacks_from_sliding(SF::square_bb(s), occupied, AttackGen::BishopDirections, 4)
                                                          | AttackGen::attacks_from_sliding(SF::square_bb(s), occupied, AttackGen::RookDirections, 4);

                // Cộng dồn vào bản đồ tấn công của loại quân này
                current_attacks |= attacks_from_s;

                // --- Tính King Safety đồng thời ---
                // Lưu ý: attacks_from_s đã được tính toán ở trên
                SF::Bitboard king_ring_attacked = attacks_from_s & kingRing[them];
                // Nếu quân cờ tại s này tấn công vào vùng Vua địch
                if (attacks_from_s & kingRing[them]) {
                    kingAttackersCount[c]++; // Tăng số quân tấn công
                    if (pt >= SF::KNIGHT && pt <= SF::QUEEN) { // Chỉ tính weight cho quân có giá trị
                        kingAttackersWeight[c] += KingAttackWeights[pt];
                    }
                }
                
            } // Kết thúc lặp qua từng quân cờ

            // Lưu lại bản đồ tấn công tổng hợp cho loại quân này
            attackedBy[c][pt] = current_attacks;
        } 
         attackedBy[c][SF::ALL_PIECES] = attackedBy[c][SF::PAWN]   | attackedBy[c][SF::KNIGHT]
                                       | attackedBy[c][SF::BISHOP] | attackedBy[c][SF::ROOK]
                                       | attackedBy[c][SF::QUEEN]  | attackedBy[c][SF::KING];

    } // Kết thúc lặp qua Color

    //--- 4. Tính toán attackedBy2 (sau khi đã có attackedBy hoàn chỉnh) ---
     for (SF::Color c : { SF::WHITE, SF::BLACK }) {
        SF::Bitboard attacks_once = 0;
        SF::Bitboard attacks_more = 0;
        // Chỉ cần lặp qua các loại quân thực sự tấn công (P-Q)
        for (SF::PieceType pt = SF::PAWN; pt <= SF::QUEEN; ++pt) {
             SF::Bitboard b = attackedBy[c][pt];
             attacks_more |= attacks_once & b;

             attacks_once |= b;
        }
        // Vua cũng góp phần vào attackedBy2
        attacks_more |= attacks_once & attackedBy[c][SF::KING];
        attackedBy2[c] = attacks_more;
    }

    // --- 5. Tính toán mobilityArea ---
     for (SF::Color c : { SF::WHITE, SF::BLACK }) {
         SF::Color them = ~c;
         // Phiên bản đơn giản: ô trống không bị Tốt địch và quân mình chiếm
         mobilityArea[c] = ~(pieces(c) | attackedBy[them][SF::PAWN]);
     }

    // Gán con trỏ
    pawn_entry_ptr = pe_ptr;
    material_entry_ptr = me_ptr;
}

// Các hàm helper khác có thể được định nghĩa ở đây nếu cần

} // namespace EvalInfo