#ifndef KING_SAFETY_H
#define KING_SAFETY_H

#include "types.h" // Cần cho Color
#include "board.h" // Cần cho S_BOARD
#include "eval_help.h" // Cần cho eval_help::Score và eval_help::make_score



eval_help::Score CalculateSimpleKingSafetyScore(const S_BOARD *pos, Color kingColor); // Đổi tên cho rõ ràng hơn nếu muốn



#endif // KING_SAFETY_H