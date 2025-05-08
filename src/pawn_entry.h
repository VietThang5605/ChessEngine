#ifndef PAWN_ENTRY_H_INCLUDED
#define PAWN_ENTRY_H_INCLUDED

#include "eval_help.h" 
#include "types.h"


struct PawnEntry {
    U64 key = 0; ///< Khóa Zobrist 64-bit chỉ dựa trên vị trí Tốt.

    // Điểm cấu trúc Tốt cho mỗi bên (kết hợp MG và EG).
    eval_help::Score scores[BOTH] = { eval_help::SCORE_ZERO, eval_help::SCORE_ZERO };

    // Bitboard chứa vị trí các Tốt thông của mỗi bên.
    U64 passedPawns[BOTH] = { 0, 0 };

    // Bitboard các ô bị Tốt mỗi bên tấn công trực tiếp.
    U64 pawnAttacks[BOTH] = { 0, 0 };

    /**
     * @brief Bitboard các ô trong tầm kiểm soát của Tốt.
     * Bao gồm các ô Tốt tấn công trực tiếp và các ô Tốt có thể tấn công
     * nếu chúng được đẩy lên một hoặc hai bước (không tính ô phong cấp).
     */
    U64 pawnAttacksSpan[BOTH] = { 0, 0 };

    // // Bitboard chứa các vị trí Tốt yếu của mỗi bên.
    // U64 weakPawns[BOTH] = { 0, 0 }; 

    // Điểm cho vua nguy hiểm (tính cho cả hai bên).
    // eval_help::Score kingSafety[BOTH] = { eval_help::SCORE_ZERO, eval_help::SCORE_ZERO };
    // U64 kingSquares[BOTH] = { NO_SQ, NO_SQ }; 
    // int castlingRights[BOTH] = { 0, 0 }; // Lưu cả quyền nhập thành khi tính key
};

#endif // PAWN_ENTRY_H_INCLUDED
