#ifndef ATTACK_GEN_H_INCLUDED
#define ATTACK_GEN_H_INCLUDED

#include "evaluation_types.h" // File chứa định nghĩa SF::...
#include "board.h"             // File chứa định nghĩa S_BOARD và U64

// Forward declaration của S_BOARD nếu cần thiết, hoặc đảm bảo defs.h được include
// struct S_BOARD; // Bỏ comment nếu defs.h không được include ở đây

namespace AttackGen {

    // --- Khai báo các bảng tấn công tính sẵn (extern) ---
    extern SF::Bitboard PseudoAttacks_KNIGHT[SF::SQUARE_NB];
    extern SF::Bitboard PseudoAttacks_KING[SF::SQUARE_NB];

    // --- KHAI BÁO EXTERN CHO CÁC MẢNG HƯỚNG ĐI ---
    extern const SF::Direction BishopDirections[4]; // Thêm extern const
    extern const SF::Direction RookDirections[4];   // Thêm extern const
    // extern const SF::Direction KingDirections[8]; // Thêm nếu cần dùng ở file khác
    // extern const int KnightOffsets[8];           // Thêm nếu cần dùng ở file khác

    // --- Khai báo hàm khởi tạo ---
    void init_attack_tables();

    // Trả về bitboard các ô bị Tốt bên 'side' tấn công
    SF::Bitboard attacks_from_pawns(const S_Board* pos, SF::Color side);

    // Trả về bitboard các ô bị Mã bên 'side' tấn công
    SF::Bitboard attacks_from_knights(const S_Board* pos, SF::Color side);

    // Trả về bitboard các ô bị Vua bên 'side' tấn công
    SF::Bitboard attacks_from_king(const S_Board* pos, SF::Color side);

    // Trả về bitboard các ô bị Tượng bên 'side' tấn công
    SF::Bitboard attacks_from_bishops(const S_Board* pos, SF::Color side);

    // Trả về bitboard các ô bị Xe bên 'side' tấn công
    SF::Bitboard attacks_from_rooks(const S_Board* pos, SF::Color side);

    // Trả về bitboard các ô bị Hậu bên 'side' tấn công
    SF::Bitboard attacks_from_queens(const S_Board* pos, SF::Color side);

    // Hàm tính tổng tấn công cho một bên
    SF::Bitboard attacks_by_side(const S_Board* pos, SF::Color side);

    // *** KHAI BÁO HÀM HELPER attacks_from_sliding ***
    // Hàm này giờ đây có thể được gọi từ bên ngoài (eval_info.cpp)
    SF::Bitboard attacks_from_sliding(SF::Bitboard sliders, SF::Bitboard occupied, const SF::Direction steps[], int numSteps);

} // namespace AttackGen

#endif // ATTACK_GEN_H_INCLUDED