#include "attack_gen.h"
#include "board.h"  
#include "bitboards.h" 
#include "eval_help.h"   




namespace AttackGen {

    // --- Định nghĩa các bảng tấn công toàn cục ---
    U64 PawnAttacks[BOTH][SQUARE_NB];


    // --- Hàm khởi tạo bảng tấn công ---
    // Gọi hàm này một lần khi engine khởi động
    void init_attack_tables() {
        for (int s_int = 0; s_int < SQUARE_NB; ++s_int) {
            eval_help::Square_pawn s = static_cast<eval_help::Square_pawn>(s_int);

            PawnAttacks[WHITE][s] = 0; // Khởi tạo bằng 0
            PawnAttacks[BLACK][s] = 0; // Khởi tạo bằng 0

            // === KHỞI TẠO PAWNATTACKS ===
                // Tấn công Tốt trắng
            eval_help::Square_pawn to_ne = static_cast<eval_help::Square_pawn>(s + eval_help::NORTH_EAST);
            eval_help::Square_pawn to_nw = static_cast<eval_help::Square_pawn>(s + eval_help::NORTH_WEST);
            if (eval_help::is_ok(to_ne) && eval_help::file_of(to_ne) > eval_help::file_of(s)) // Kiểm tra tràn cột H
                PawnAttacks[WHITE][s] |= eval_help::square_bb(to_ne);
            if (eval_help::is_ok(to_nw) && eval_help::file_of(to_nw) < eval_help::file_of(s)) // Kiểm tra tràn cột A
                PawnAttacks[WHITE][s] |= eval_help::square_bb(to_nw);

                // Tấn công Tốt đen
            eval_help::Square_pawn to_se = static_cast<eval_help::Square_pawn>(s + eval_help::SOUTH_EAST);
            eval_help::Square_pawn to_sw = static_cast<eval_help::Square_pawn>(s + eval_help::SOUTH_WEST);
            if (eval_help::is_ok(to_se) && eval_help::file_of(to_se) > eval_help::file_of(s)) // Kiểm tra tràn cột H
                PawnAttacks[BLACK][s] |= eval_help::square_bb(to_se);
            if (eval_help::is_ok(to_sw) && eval_help::file_of(to_sw) < eval_help::file_of(s)) // Kiểm tra tràn cột A
                PawnAttacks[BLACK][s] |= eval_help::square_bb(to_sw);
        }

    }

    // --- Triển khai các hàm tính toán tấn công (sử dụng S_BOARD*) ---
    U64 attacks_from_pawns(const S_BOARD* pos, Color side) {
        U64 pawns = pos->pawnsBB[side]; // Lấy bitboard Tốt từ S_BOARD
        U64 attacks = 0;
            // Tối ưu hơn là dùng bảng tính sẵn
        while (pawns) {
            attacks |= PawnAttacks[side][eval_help::pop_lsb(&pawns)];
        }
        return attacks;
    }

} // namespace AttackGen