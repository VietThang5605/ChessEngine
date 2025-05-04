#ifndef PAWN_ENTRY_H_INCLUDED
#define PAWN_ENTRY_H_INCLUDED

#include "evaluation_types.h" // Cần cho SF::Score, SF::Bitboard, SF::Color, SF::Square, SF::Key

/**
 * @struct PawnEntry
 * @brief Lưu trữ kết quả phân tích cấu trúc Tốt cho một thế cờ.
 *
 * Cấu trúc này chứa các thông tin được tính toán trước như điểm số cấu trúc Tốt,
 * các Tốt thông, và các bitboard tấn công của Tốt. Nó được lưu trữ trong
 * PawnHashTable để tránh tính toán lại.
 */
struct PawnEntry {
    SF::Key key = 0; ///< Khóa Zobrist 64-bit chỉ dựa trên vị trí Tốt.

    ///< Điểm cấu trúc Tốt cho mỗi bên (kết hợp MG và EG).
    SF::Score scores[SF::COLOR_NB] = { SF::SCORE_ZERO, SF::SCORE_ZERO };

    ///< Bitboard chứa vị trí các Tốt thông của mỗi bên.
    SF::Bitboard passedPawns[SF::COLOR_NB] = { 0, 0 };

    ///< Bitboard các ô bị Tốt mỗi bên tấn công trực tiếp.
    SF::Bitboard pawnAttacks[SF::COLOR_NB] = { 0, 0 };

    /**
     * @brief Bitboard các ô trong tầm kiểm soát của Tốt.
     * Bao gồm các ô Tốt tấn công trực tiếp và các ô Tốt có thể tấn công
     * nếu chúng được đẩy lên một hoặc hai bước (không tính ô phong cấp).
     */
    SF::Bitboard pawnAttacksSpan[SF::COLOR_NB] = { 0, 0 };

    // Các thành viên tùy chọn có thể thêm sau để lưu trữ chi tiết hơn, ví dụ:
    // SF::Bitboard candidatePawns[SF::COLOR_NB] = { 0, 0 }; // Tốt ứng viên thông
    // SF::Bitboard weakPawns[SF::COLOR_NB] = { 0, 0 }; // Ví dụ: isolated | backward

    SF::Score kingSafety[SF::COLOR_NB] = { SF::SCORE_ZERO, SF::SCORE_ZERO };
    SF::Square kingSquares[SF::COLOR_NB] = { SF::SQ_NONE, SF::SQ_NONE };
    int castlingRights[SF::COLOR_NB] = { 0, 0 }; // Lưu cả quyền nhập thành khi tính key
};

#endif // PAWN_ENTRY_H_INCLUDED
