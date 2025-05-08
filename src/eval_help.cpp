#include "eval_help.h"
#include "attack_gen.h" // Cần cho SF::pawn_attack_span nếu nó dùng AttackGen::PawnAttacks
#include <iostream>

namespace eval_help {
    // --- Định nghĩa các mảng toàn cục ---
    U64 SquareBB[SQUARE_NB];

    // --- Hàm khởi tạo các mảng ---
    void init_U64_utils() {
        for (Square_pawn s = SQ_A1; s <= SQ_H8; ++s) {
            SquareBB[s] = (U64(1) << s);
        }
    }
    // === ĐỊNH NGHĨA CÁC HÀM TIỆN ÍCH U64 (đã khai báo extern) ===

    U64 adjacent_files_bb(eval_help::Square_pawn s) {
        File f = file_of(s); // Lấy file của ô s (0 cho A, 1 cho B, ..., 7 cho H)
        U64 adjacentMask = 0ULL;

        // Mảng các FileMasks
        // Khai báo static const để mảng chỉ được khởi tạo một lần.
        static const U64 FileMasks[] = {
            FileABB, FileBBB, FileCBB, FileDBB, FileEBB, FileFBB, FileGBB, FileHBB
        };

        // Thêm cột bên trái (nếu có)
        if (f > FILE_A) { // Nếu không phải cột A (giả sử FILE_A là 0)
            adjacentMask |= FileMasks[f - 1]; // Lấy FileMasks[f-1] cho cột bên trái
        }

        // Thêm cột bên phải (nếu có)
        if (f <FILE_H) { // Nếu không phải cột H (giả sử FILE_H là 7)
            adjacentMask |= FileMasks[f + 1]; // Lấy FileMasks[f+1] cho cột bên phải
        }

        return adjacentMask;
    }

    U64 rank_bb(Rank r) {
        // Đảm bảo Rank1BB được định nghĩa trong evaluation_types.h
        return Rank1BB << (8 * r);
    }

    U64 rank_bb(eval_help::Square_pawn s) {
        return rank_bb(rank_of(s));
    }

    U64 file_bb(File f) {
        // Đảm bảo FileABB được định nghĩa trong evaluation_types.h
        return FileABB << f;
    }

    U64 file_bb(eval_help::Square_pawn s) {
        return file_bb(file_of(s));
    }

    U64 forward_ranks_bb(Color c, eval_help::Square_pawn s) {
         // Đảm bảo AllSquares được định nghĩa trong evaluation_types.h
         return (c == WHITE) ? (AllSquares << (rank_of(s) * 8 + 8))
                             : (AllSquares >> ((7 - rank_of(s)) * 8 + 8));
    }

    U64 forward_file_bb(Color c, eval_help::Square_pawn s) {
        return forward_ranks_bb(c, s) & file_bb(s);
    }

    U64 passed_pawn_span(Color c, eval_help::Square_pawn s) {
        return forward_ranks_bb(c, s) & (adjacent_files_bb(s) | file_bb(s));
    }

    U64 pawn_attack_span(Color c, eval_help::Square_pawn s) {
        U64 pushes = forward_file_bb(c, s);
        U64 targets = 0;
        while(pushes) {
            eval_help::Square_pawn to = pop_lsb(&pushes);
            // Không tính tấn công từ ô phong cấp
            if (relative_rank(c, rank_of(to)) != RANK_8) {
                 // Cần truy cập mảng PawnAttacks toàn cục
                 // Đảm bảo AttackGen::PawnAttacks được khai báo extern và định nghĩa đúng
                 targets |= AttackGen::PawnAttacks[c][to];
            }
        }
        return targets;
    }

    // more_than_one và popcount đã được định nghĩa inline trong header v3

} // namespace SF
