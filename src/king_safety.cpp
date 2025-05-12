#include "king_safety.h"
#include "init.h"    // For FileBBMask (used via eval_help::file_bb)
#include "data.h"      // For Color, Piece enums etc. (indirectly via types.h)
// #include "movegen.h" // Not directly used in this snippet, but good for context
#include "eval_help.h" // For Score, Square_pawn, file_of, rank_of, square_bb, Directions

// --- Các hằng số đánh giá an toàn vua (sử dụng MG/EG) ---
namespace { 
    #define S(mg, eg) eval_help::make_score(mg, eg)
    // constexpr eval_help::Score KING_SAFETY_PAWN_SHIELD_ONE_STEP = S(20, 0); // cũ: 15,0
    constexpr eval_help::Score KING_SAFETY_PAWN_SHIELD_ONE_STEP = S(30, 0); // tăng mạnh
    // constexpr eval_help::Score KING_SAFETY_PAWN_SHIELD_TWO_STEPS = S(10, 0); // cũ: 8,0
    constexpr eval_help::Score KING_SAFETY_PAWN_SHIELD_TWO_STEPS = S(18, 0); // tăng mạnh
    // constexpr eval_help::Score KING_SAFETY_SHIELD_ON_ADJACENT_FILE_PENALTY = S(-8, 0); // cũ: -10,0
    constexpr eval_help::Score KING_SAFETY_SHIELD_ON_ADJACENT_FILE_PENALTY = S(-20, 0); // phạt mạnh hơn
    // constexpr eval_help::Score KING_SAFETY_OPEN_FILE_IN_FRONT_PENALTY = S(-35, 0); // cũ: -25,0
    constexpr eval_help::Score KING_SAFETY_OPEN_FILE_IN_FRONT_PENALTY = S(-60, 0); // phạt rất mạnh
    // constexpr eval_help::Score KING_SAFETY_ENEMY_PAWN_NEAR_PENALTY = S(-18, 0); // cũ: -12,0
    constexpr eval_help::Score KING_SAFETY_ENEMY_PAWN_NEAR_PENALTY = S(-40, 0); // phạt mạnh
    // constexpr eval_help::Score KING_SAFETY_ENEMY_PAWN_VERY_NEAR_PENALTY = S(-32, 0); // cũ: -24,0
    constexpr eval_help::Score KING_SAFETY_ENEMY_PAWN_VERY_NEAR_PENALTY = S(-70, 0); // phạt cực mạnh
    // constexpr eval_help::Score KING_ON_OPEN_FILE_PENALTY = S(-50, -30); // cũ: -40,-20
    constexpr eval_help::Score KING_ON_OPEN_FILE_PENALTY = S(-90, -60); // phạt cực mạnh
    // constexpr eval_help::Score KING_ON_SEMI_OPEN_FILE_PENALTY = S(-25, -15); // cũ: -20,-10
    constexpr eval_help::Score KING_ON_SEMI_OPEN_FILE_PENALTY = S(-45, -30); // phạt mạnh hơn
    #undef S
}

// --- Hàm chính tính điểm an toàn vua ---
eval_help::Score CalculateSimpleKingSafetyScore(const S_BOARD *pos, Color kingColor) {
    ASSERT(CheckBoard(pos));

    eval_help::Score score = eval_help::SCORE_ZERO;
    int kingSq120 = pos->kingSquare[kingColor];

    if (kingSq120 == NO_SQ) {
        return eval_help::SCORE_ZERO;
    }

    // Chuyển sang tọa độ 64-bit một lần
    eval_help::Square_pawn kingSq64 = static_cast<eval_help::Square_pawn>(Sq120ToSq64[kingSq120]);
    File king_eh_file = eval_help::file_of(kingSq64);
    Rank king_eh_rank = eval_help::rank_of(kingSq64); 

    Color enemyColor = (kingColor == WHITE) ? BLACK : WHITE;

    // PawnShield
    if (king_eh_file <= FILE_C || king_eh_file >=FILE_F) {
        bool missing_diag_left_fwd_pawn = false;
        bool missing_diag_right_fwd_pawn = false;
        for (int file_offset = -1; file_offset <= 1; ++file_offset) {
            File current_shield_file = static_cast<File>(king_eh_file + file_offset);

            if (current_shield_file < FILE_A || current_shield_file > FILE_H) continue;

            // Ô lá chắn thứ nhất (cách Vua 1 hàng)
            Rank shield_sq1_eh_rank = static_cast<Rank>(king_eh_rank + ((kingColor == WHITE) ? 1 : -1));
            if (shield_sq1_eh_rank >= RANK_1 && shield_sq1_eh_rank <= RANK_8) {
                eval_help::Square_pawn shield_sq1 = static_cast<eval_help::Square_pawn>(shield_sq1_eh_rank * 8 + current_shield_file);

                if (pos->pawnsBB[kingColor] & eval_help::square_bb(shield_sq1)) {
                    score += (file_offset == 0) ? KING_SAFETY_PAWN_SHIELD_ONE_STEP
                                            : KING_SAFETY_PAWN_SHIELD_ONE_STEP + KING_SAFETY_SHIELD_ON_ADJACENT_FILE_PENALTY;
                } else {
                    // Nếu không có Tốt ở ô lá chắn thứ nhất, và ô đó nằm ngay trước mặt Vua
                    if (file_offset == 0) {
                        U64 front_squares_mask = eval_help::square_bb(shield_sq1); // Ô cách 1 hàng
                        Rank rank_two_steps_ahead_eh = static_cast<Rank>(king_eh_rank + ((kingColor == WHITE) ? 2 : -2));

                        if (rank_two_steps_ahead_eh >= RANK_1 && rank_two_steps_ahead_eh <= RANK_8) {
                            eval_help::Square_pawn sq_two_steps_ahead_64 = static_cast<eval_help::Square_pawn>(rank_two_steps_ahead_eh * 8 + current_shield_file);
                            front_squares_mask |= eval_help::square_bb(sq_two_steps_ahead_64); // Thêm ô cách 2 hàng
                        }

                        // Nếu không có Tốt phe ta trên cả 2 ô này (ô cách 1 hàng và ô cách 2 hàng ngay trước mặt)
                        if ((pos->pawnsBB[kingColor] & front_squares_mask) == 0ULL) {
                            score += KING_SAFETY_OPEN_FILE_IN_FRONT_PENALTY;
                        }
                    }
                    if (file_offset == -1) {
                        missing_diag_left_fwd_pawn = true;
                    }
                    if (file_offset == 1) {
                        missing_diag_right_fwd_pawn = true;
                    }
                }
            }

            // Ô lá chắn thứ hai (cách Vua 2 hàng)
            Rank shield_sq2_eh_rank = static_cast<Rank>(king_eh_rank + ((kingColor == WHITE) ? 2 : -2));
            if (shield_sq2_eh_rank >= RANK_1 && shield_sq2_eh_rank <= RANK_8) {
                eval_help::Square_pawn shield_sq2 = static_cast<eval_help::Square_pawn>(shield_sq2_eh_rank * 8 + current_shield_file);
                if (pos->pawnsBB[kingColor] & eval_help::square_bb(shield_sq2)) {
                    score += (file_offset == 0) ? KING_SAFETY_PAWN_SHIELD_TWO_STEPS
                                            : KING_SAFETY_PAWN_SHIELD_TWO_STEPS + KING_SAFETY_SHIELD_ON_ADJACENT_FILE_PENALTY;
                }
            }
        }
        if (missing_diag_left_fwd_pawn ==true && missing_diag_right_fwd_pawn == true) {
            // Nếu không có Tốt nào ở 2 ô chéo trước mặt Vua
            score += KING_SAFETY_SHIELD_ON_ADJACENT_FILE_PENALTY;
        }
    }
    

    // Pawn Storm
    for (int file_offset = -1; file_offset <= 1; ++file_offset) {
        File current_storm_file = static_cast<File>(king_eh_file + file_offset);

        if (current_storm_file < FILE_A || current_storm_file > FILE_H) continue;

        // Tốt địch ở gần
        Rank enemy_pawn_near_eh_rank = static_cast<Rank>(king_eh_rank + ((kingColor == WHITE) ? 2 : -2));
        if (enemy_pawn_near_eh_rank >= RANK_1 && enemy_pawn_near_eh_rank <= RANK_8) {
            eval_help::Square_pawn enemy_sq_near = static_cast<eval_help::Square_pawn>(enemy_pawn_near_eh_rank * 8 + current_storm_file);
            if (pos->pawnsBB[enemyColor] & eval_help::square_bb(enemy_sq_near)) {
                score += KING_SAFETY_ENEMY_PAWN_NEAR_PENALTY;
            }
        }

        // Tốt địch ở rất gần
        Rank enemy_pawn_very_near_eh_rank = static_cast<Rank>(king_eh_rank + ((kingColor == WHITE) ? 1 : -1));
        if (enemy_pawn_very_near_eh_rank >= RANK_1 && enemy_pawn_very_near_eh_rank <= RANK_8) {
            eval_help::Square_pawn enemy_sq_very_near = static_cast<eval_help::Square_pawn>(enemy_pawn_very_near_eh_rank * 8 + current_storm_file);
            if (pos->pawnsBB[enemyColor] & eval_help::square_bb(enemy_sq_very_near)) {
                score += KING_SAFETY_ENEMY_PAWN_VERY_NEAR_PENALTY;
            }
        }
    }

    // 3. Đánh giá cột vua đang đứng
    U64 king_file_bitboard = FileBBMask[king_eh_file]; // FileBBMask từ init.h, index bằng enum File (0-7)

    if ((pos->pawnsBB[BOTH] & king_file_bitboard) == 0ULL) {
            // Cột vua đứng hoàn toàn không có Tốt nào -> Cột mở
        score += KING_ON_OPEN_FILE_PENALTY;
    } else if (eval_help::popcount(pos->pawnsBB[BOTH] & king_file_bitboard) == 1) {
        // Cột vua đứng ỏn có 1 Tốt -> Cột bán mở
        score += KING_ON_SEMI_OPEN_FILE_PENALTY;
    }

    return score;
}