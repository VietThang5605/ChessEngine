#include "king_safety.h"
#include "init.h"    
#include "data.h"      
#include "movegen.h"   
#include "eval_help.h" 

// --- Các hằng số đánh giá an toàn vua (sử dụng MG/EG) ---
namespace { 
    #define S(mg, eg) eval_help::make_score(mg, eg)
    constexpr eval_help::Score KING_SAFETY_PAWN_SHIELD_ONE_STEP = S(10, 0);
    constexpr eval_help::Score KING_SAFETY_PAWN_SHIELD_TWO_STEPS = S(5, 0); 
    constexpr eval_help::Score KING_SAFETY_SHIELD_ON_ADJACENT_FILE_PENALTY = S(-8, 0);
    constexpr eval_help::Score KING_SAFETY_OPEN_FILE_IN_FRONT_PENALTY = S(-15, 0); 
    constexpr eval_help::Score KING_SAFETY_ENEMY_PAWN_NEAR_PENALTY = S(-10, 0);
    constexpr eval_help::Score KING_SAFETY_ENEMY_PAWN_VERY_NEAR_PENALTY = S(-20, 0);
    constexpr eval_help::Score KING_ON_OPEN_FILE_PENALTY = S(-30, 0); 
    constexpr eval_help::Score KING_ON_SEMI_OPEN_FILE_PENALTY = S(-15, 0); 
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

    int kingFile = FilesBrd[kingSq120];
    int kingRank = RanksBrd[kingSq120];
    Color enemyColor = (kingColor == WHITE) ? BLACK : WHITE;

    // PawnShield
    if (kingFile <= FILE_C || kingFile >= FILE_F) {
        for (int fileOffset = -1; fileOffset <= 1; ++fileOffset) {
            int currentFileShield = kingFile + fileOffset;
            if (currentFileShield < FILE_A || currentFileShield > FILE_H) continue;

            int shieldSq1_rank = kingRank + ((kingColor == WHITE) ? 1 : -1);
            if (shieldSq1_rank >= RANK_1 && shieldSq1_rank <= RANK_8) {
                int shieldSq1_120 = FR2SQ(currentFileShield, shieldSq1_rank);
                if (!SQOFFBOARD(shieldSq1_120)) {
                    if (pos->pawnsBB[kingColor] & SetMask[Sq120ToSq64[shieldSq1_120]]) {
                        score += (fileOffset == 0) ? KING_SAFETY_PAWN_SHIELD_ONE_STEP
                                                : KING_SAFETY_PAWN_SHIELD_ONE_STEP + KING_SAFETY_SHIELD_ON_ADJACENT_FILE_PENALTY;
                    } else {
                        if (fileOffset == 0) { 
                            U64 open_file_check_mask = 0ULL;
                            for (int rLookAhead = 1; rLookAhead <= 2; ++rLookAhead) {
                                int rankToTest = kingRank + ((kingColor == WHITE) ? rLookAhead : -rLookAhead);
                                if (rankToTest < RANK_1 || rankToTest > RANK_8) break;
                                int sqToTest120 = FR2SQ(currentFileShield, rankToTest);
                                if (!SQOFFBOARD(sqToTest120)) {
                                    open_file_check_mask |= SetMask[Sq120ToSq64[sqToTest120]];
                                }
                            }
                            if ((pos->pawnsBB[kingColor] & open_file_check_mask) == 0ULL) {
                                score += KING_SAFETY_OPEN_FILE_IN_FRONT_PENALTY;
                            }
                        }
                    }
                }
            }

            int shieldSq2_rank = kingRank + ((kingColor == WHITE) ? 2 : -2);
            if (shieldSq2_rank >= RANK_1 && shieldSq2_rank <= RANK_8) {
                int shieldSq2_120 = FR2SQ(currentFileShield, shieldSq2_rank);
                if (!SQOFFBOARD(shieldSq2_120)) {
                    if (pos->pawnsBB[kingColor] & SetMask[Sq120ToSq64[shieldSq2_120]]) {
                        score += (fileOffset == 0) ? KING_SAFETY_PAWN_SHIELD_TWO_STEPS
                                                : KING_SAFETY_PAWN_SHIELD_TWO_STEPS + KING_SAFETY_SHIELD_ON_ADJACENT_FILE_PENALTY;
                    }
                }
            }
        } 
    }

    // Pawn Storm
    for (int fileOffset = -1; fileOffset <= 1; ++fileOffset) {
        int currentFileStorm = kingFile + fileOffset; 
        if (currentFileStorm < FILE_A || currentFileStorm > FILE_H) continue;

        int enemyPawnNearRank = kingRank + ((kingColor == WHITE) ? 1 : -1);
        if (enemyPawnNearRank >= RANK_1 && enemyPawnNearRank <= RANK_8) {
            int enemySq_120 = FR2SQ(currentFileStorm, enemyPawnNearRank);
            if (!SQOFFBOARD(enemySq_120)) {
                if (pos->pawnsBB[enemyColor] & SetMask[Sq120ToSq64[enemySq_120]]) {
                    score += KING_SAFETY_ENEMY_PAWN_VERY_NEAR_PENALTY;
                }
            }
        }

        int enemyPawnVeryNearRank = kingRank + ((kingColor == WHITE) ? 2 : -2);
         if (enemyPawnVeryNearRank >= RANK_1 && enemyPawnVeryNearRank <= RANK_8) {
             int enemySq_120 = FR2SQ(currentFileStorm, enemyPawnVeryNearRank);
             if (!SQOFFBOARD(enemySq_120)) {
                if (pos->pawnsBB[enemyColor] & SetMask[Sq120ToSq64[enemySq_120]]) {
                    score += KING_SAFETY_ENEMY_PAWN_NEAR_PENALTY;
                }
            }
        }
    }

    // 3. Đánh giá cột vua đang đứng 
        U64 kingFileBitboard = FileBBMask[kingFile];

        if ((pos->pawnsBB[BOTH] & kingFileBitboard) == 0ULL) {
            // Cột vua đứng hoàn toàn không có Tốt nào  -> Cột mở
            score += KING_ON_OPEN_FILE_PENALTY;
        } else if (eval_help::popcount(pos->pawnsBB[BOTH] & kingFileBitboard) == 1) {
            // Cột vua đứng ỏn có 1 Tốt -> Cột bán mở
            score += KING_ON_SEMI_OPEN_FILE_PENALTY;
        }
    
    return score;
}



// #include "king_safety.h"
// #include "init.h"    // For FileBBMask (used via eval_help::file_bb)
// #include "data.h"      // For Color, Piece enums etc. (indirectly via types.h)
// // #include "movegen.h" // Not directly used in this snippet, but good for context
// #include "eval_help.h" // For Score, Square_pawn, file_of, rank_of, square_bb, Directions

// // --- Các hằng số đánh giá an toàn vua (sử dụng MG/EG) ---
// namespace {
//     #define S(mg, eg) eval_help::make_score(mg, eg)
//     constexpr eval_help::Score KING_SAFETY_PAWN_SHIELD_ONE_STEP = S(12, 0);
//     constexpr eval_help::Score KING_SAFETY_PAWN_SHIELD_TWO_STEPS = S(7, 0);
//     constexpr eval_help::Score KING_SAFETY_SHIELD_ON_ADJACENT_FILE_PENALTY = S(-6, 0);
//     constexpr eval_help::Score KING_SAFETY_OPEN_FILE_IN_FRONT_PENALTY = S(-15, 0);
//     constexpr eval_help::Score KING_SAFETY_ENEMY_PAWN_NEAR_PENALTY = S(-8, 0);
//     constexpr eval_help::Score KING_SAFETY_ENEMY_PAWN_VERY_NEAR_PENALTY = S(-15, 0);
//     constexpr eval_help::Score KING_ON_OPEN_FILE_PENALTY = S(-20, -10);
//     constexpr eval_help::Score KING_ON_SEMI_OPEN_FILE_PENALTY = S(-10, -5);
//     #undef S
// }

// // --- Hàm chính tính điểm an toàn vua ---
// eval_help::Score CalculateSimpleKingSafetyScore(const S_BOARD *pos, Color kingColor) {
//     ASSERT(CheckBoard(pos));

//     eval_help::Score score = eval_help::SCORE_ZERO;
//     int kingSq120 = pos->kingSquare[kingColor];

//     if (kingSq120 == NO_SQ) {
//         return eval_help::SCORE_ZERO;
//     }

//     // Chuyển sang tọa độ 64-bit (eval_help::Square_pawn) một lần
//     eval_help::Square_pawn kingSq64 = static_cast<eval_help::Square_pawn>(Sq120ToSq64[kingSq120]);
//     eval_help::File king_eh_file = eval_help::file_of(kingSq64);
//     eval_help::Rank king_eh_rank = eval_help::rank_of(kingSq64); // Giá trị enum Rank (0-7)

//     Color enemyColor = (kingColor == WHITE) ? BLACK : WHITE;

//     // PawnShield
//     for (int file_offset = -1; file_offset <= 1; ++file_offset) {
//         eval_help::File current_shield_file = static_cast<eval_help::File>(king_eh_file + file_offset);

//         // Kiểm tra file hợp lệ (0-7)
//         if (current_shield_file < eval_help::FILE_A || current_shield_file > eval_help::FILE_H) continue;

//         // Ô lá chắn thứ nhất (cách Vua 1 hàng)
//         eval_help::Rank shield_sq1_eh_rank = static_cast<eval_help::Rank>(king_eh_rank + ((kingColor == WHITE) ? 1 : -1));
//         if (shield_sq1_eh_rank >= eval_help::RANK_1 && shield_sq1_eh_rank <= eval_help::RANK_8) { // Kiểm tra rank hợp lệ (0-7)
//             eval_help::Square_pawn shield_sq1 = static_cast<eval_help::Square_pawn>(shield_sq1_eh_rank * 8 + current_shield_file);

//             if (pos->pawnsBB[kingColor] & eval_help::square_bb(shield_sq1)) {
//                 score += (file_offset == 0) ? KING_SAFETY_PAWN_SHIELD_ONE_STEP
//                                            : KING_SAFETY_PAWN_SHIELD_ONE_STEP + KING_SAFETY_SHIELD_ON_ADJACENT_FILE_PENALTY;
//             } else {
//                 // Nếu không có Tốt ở ô lá chắn thứ nhất, và ô đó nằm ngay trước mặt Vua
//                 if (file_offset == 0) {
//                     U64 front_squares_mask = eval_help::square_bb(shield_sq1); // Ô cách 1 hàng
//                     eval_help::Rank rank_two_steps_ahead_eh = static_cast<eval_help::Rank>(king_eh_rank + ((kingColor == WHITE) ? 2 : -2));

//                     if (rank_two_steps_ahead_eh >= eval_help::RANK_1 && rank_two_steps_ahead_eh <= eval_help::RANK_8) {
//                         eval_help::Square_pawn sq_two_steps_ahead_64 = static_cast<eval_help::Square_pawn>(rank_two_steps_ahead_eh * 8 + current_shield_file);
//                         front_squares_mask |= eval_help::square_bb(sq_two_steps_ahead_64); // Thêm ô cách 2 hàng
//                     }

//                     // Nếu không có Tốt phe ta trên cả 2 ô này (ô cách 1 hàng và ô cách 2 hàng ngay trước mặt)
//                     if ((pos->pawnsBB[kingColor] & front_squares_mask) == 0ULL) {
//                         score += KING_SAFETY_OPEN_FILE_IN_FRONT_PENALTY;
//                     }
//                 }
//             }
//         }

//         // Ô lá chắn thứ hai (cách Vua 2 hàng)
//         eval_help::Rank shield_sq2_eh_rank = static_cast<eval_help::Rank>(king_eh_rank + ((kingColor == WHITE) ? 2 : -2));
//         if (shield_sq2_eh_rank >= eval_help::RANK_1 && shield_sq2_eh_rank <= eval_help::RANK_8) { // Kiểm tra rank hợp lệ (0-7)
//             eval_help::Square_pawn shield_sq2 = static_cast<eval_help::Square_pawn>(shield_sq2_eh_rank * 8 + current_shield_file);
//             if (pos->pawnsBB[kingColor] & eval_help::square_bb(shield_sq2)) {
//                 score += (file_offset == 0) ? KING_SAFETY_PAWN_SHIELD_TWO_STEPS
//                                            : KING_SAFETY_PAWN_SHIELD_TWO_STEPS + KING_SAFETY_SHIELD_ON_ADJACENT_FILE_PENALTY;
//             }
//         }
//     }

//     // Pawn Storm
//     for (int file_offset = -1; file_offset <= 1; ++file_offset) {
//         eval_help::File current_storm_file = static_cast<eval_help::File>(king_eh_file + file_offset);

//         if (current_storm_file < eval_help::FILE_A || current_storm_file > eval_help::FILE_H) continue;

//         // Tốt địch ở gần (cách Vua 2 hàng theo hướng tiến của Tốt địch)
//         eval_help::Rank enemy_pawn_near_eh_rank = static_cast<eval_help::Rank>(king_eh_rank + ((kingColor == WHITE) ? 2 : -2));
//         if (enemy_pawn_near_eh_rank >= eval_help::RANK_1 && enemy_pawn_near_eh_rank <= eval_help::RANK_8) {
//             eval_help::Square_pawn enemy_sq_near = static_cast<eval_help::Square_pawn>(enemy_pawn_near_eh_rank * 8 + current_storm_file);
//             if (pos->pawnsBB[enemyColor] & eval_help::square_bb(enemy_sq_near)) {
//                 score += KING_SAFETY_ENEMY_PAWN_NEAR_PENALTY;
//             }
//         }

//         // Tốt địch ở rất gần (cách Vua 3 hàng theo hướng tiến của Tốt địch)
//         eval_help::Rank enemy_pawn_very_near_eh_rank = static_cast<eval_help::Rank>(king_eh_rank + ((kingColor == WHITE) ? 3 : -3));
//         if (enemy_pawn_very_near_eh_rank >= eval_help::RANK_1 && enemy_pawn_very_near_eh_rank <= eval_help::RANK_8) {
//             eval_help::Square_pawn enemy_sq_very_near = static_cast<eval_help::Square_pawn>(enemy_pawn_very_near_eh_rank * 8 + current_storm_file);
//             if (pos->pawnsBB[enemyColor] & eval_help::square_bb(enemy_sq_very_near)) {
//                 score += KING_SAFETY_ENEMY_PAWN_VERY_NEAR_PENALTY;
//             }
//         }
//     }

//     // 3. Đánh giá cột vua đang đứng
//     if (king_eh_file >= eval_help::FILE_A && king_eh_file <= eval_help::FILE_H) { // Đã kiểm tra ở trên nhưng để cho rõ ràng
//         U64 king_file_bitboard = FileBBMask[king_eh_file]; // FileBBMask từ init.h, index bằng enum File (0-7)

//         if ((pos->pawnsBB[BOTH] & king_file_bitboard) == 0ULL) {
//             // Cột vua đứng hoàn toàn không có Tốt nào -> Cột mở
//             score += KING_ON_OPEN_FILE_PENALTY;
//         } else if ((pos->pawnsBB[kingColor] & king_file_bitboard) == 0ULL) {
//             // Cột vua đứng không có Tốt ta, nhưng có Tốt địch -> Cột nửa mở cho địch
//             score += KING_ON_SEMI_OPEN_FILE_PENALTY;
//         }
//     }

//     return score;
// }