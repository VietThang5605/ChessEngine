#ifndef ATTACK_GEN_H_INCLUDED
#define ATTACK_GEN_H_INCLUDED

#include "board.h"            


namespace AttackGen {

    // --- Khai báo các bảng tấn công tính sẵn (extern) ---
    extern U64 PawnAttacks[BOTH][SQUARE_NB];


    // --- Khai báo hàm khởi tạo ---
    void init_attack_tables();

    // Trả về bitboard các ô bị Tốt bên 'side' tấn công
    U64 attacks_from_pawns(const S_BOARD* pos, Color side);

} // namespace AttackGen

#endif // ATTACK_GEN_H_INCLUDED