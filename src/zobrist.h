#ifndef ZOBRIST_H_INCLUDED
#define ZOBRIST_H_INCLUDED

#include "evaluation_types.h" // Cho SF::Key, SF::Piece, SF::Square

namespace Zobrist {
    // Mảng key cho từng quân cờ trên từng ô (chỉ cần cho Tốt nếu chỉ tính pawnKey)
    extern SF::Key Psq[13][64];
    // Thêm các key khác nếu bạn muốn tính posKey đầy đủ sau này
    // extern SF::Key Enpassant[SF::FILE_NB];
    // extern SF::Key Castling[16];
    // extern SF::Key Side;

    // Hàm khởi tạo các số Zobrist
    void init();
}
#endif // ZOBRIST_H_INCLUDED