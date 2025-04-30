#ifndef EVALUATE_PHASE3_H_INCLUDED
#define EVALUATE_PHASE3_H_INCLUDED

#include "board.h"            // Định nghĩa S_Board
#include "evaluation_types.h" // Định nghĩa SF::Value, SF::Score, etc.

// --- Khai báo hàm cần thiết ---

// Hàm đánh giá chính
// Trả về điểm số theo góc nhìn của bên đang đi (pos->side)
SF::Value EvaluatePosition(const S_Board* pos);

// (Tùy chọn) Namespace riêng cho việc đánh giá ở giai đoạn 3
namespace EvalPhase3 {
    // Có thể đặt các hàm helper hoặc lớp nội bộ ở đây nếu cần
}

#endif // EVALUATE_PHASE3_H_INCLUDED