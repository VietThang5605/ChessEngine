#ifndef PSQT_H_INCLUDED
#define PSQT_H_INCLUDED

#include "evaluation_types.h" 

// --- Khai báo biến toàn cục (dùng extern) ---
// Khai báo mảng giá trị quân cờ. Định nghĩa nằm trong psqt.cpp
extern SF::Value Piecevalue[SF::PHASE_NB][SF::PIECE_NB];

// --- Khai báo trong namespace PSQT ---
namespace PSQT {

    // Khai báo mảng điểm vị trí quân cờ (Piece-Square Table). Định nghĩa nằm trong psqt.cpp
    extern SF::Score psq[SF::PIECE_NB][SF::SQUARE_NB];

    // Khai báo hàm khởi tạo. Định nghĩa nằm trong psqt.cpp
    extern void init();

} // namespace PSQT

#endif // PSQT_H_INCLUDED