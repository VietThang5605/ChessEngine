#include "evaluation_types.h"
#include "attack_gen.h" // Cần cho SF::pawn_attack_span nếu nó dùng AttackGen::PawnAttacks

namespace SF {
    // --- Định nghĩa các mảng toàn cục ---
    uint8_t SquareDistance[SQUARE_NB][SQUARE_NB];
    Bitboard SquareBB[SQUARE_NB];

    // --- Hàm khởi tạo các mảng ---
    void init_bitboard_utils() {
        // Khởi tạo SquareDistance
        for (Square s1 = SQ_A1; s1 <= SQ_H8; ++s1) {
            for (Square s2 = SQ_A1; s2 <= SQ_H8; ++s2) {
                SquareDistance[s1][s2] = static_cast<uint8_t>(
                    std::max(distance<File>(s1, s2), distance<Rank>(s1, s2))
                );
            }
        }
        // Khởi tạo SquareBB
        for (Square s = SQ_A1; s <= SQ_H8; ++s) {
            SquareBB[s] = (Bitboard(1) << s);
        }
    }

    // === ĐỊNH NGHĨA CÁC HÀM TIỆN ÍCH BITBOARD (đã khai báo extern) ===

    Bitboard adjacent_files_bb(Square s) {
        Bitboard b = square_bb(s);
        // Đảm bảo FileABB và FileHBB được định nghĩa trong evaluation_types.h
        Bitboard east = (b & ~FileHBB) << 1;
        Bitboard west = (b & ~FileABB) >> 1;
        return east | west;
    }

    Bitboard rank_bb(Rank r) {
        // Đảm bảo Rank1BB được định nghĩa trong evaluation_types.h
        return Rank1BB << (8 * r);
    }

    Bitboard rank_bb(Square s) {
        return rank_bb(rank_of(s));
    }

    Bitboard file_bb(File f) {
        // Đảm bảo FileABB được định nghĩa trong evaluation_types.h
        return FileABB << f;
    }

    Bitboard file_bb(Square s) {
        return file_bb(file_of(s));
    }

    Bitboard forward_ranks_bb(Color c, Square s) {
         // Đảm bảo AllSquares được định nghĩa trong evaluation_types.h
         return (c == WHITE) ? (AllSquares << (rank_of(s) * 8 + 8))
                             : (AllSquares >> ((7 - rank_of(s)) * 8 + 8));
    }

    Bitboard forward_file_bb(Color c, Square s) {
        return forward_ranks_bb(c, s) & file_bb(s);
    }

    Bitboard passed_pawn_span(Color c, Square s) {
        return forward_ranks_bb(c, s) & (adjacent_files_bb(s) | file_bb(s));
    }

    Bitboard pawn_attack_span(Color c, Square s) {
        Bitboard pushes = forward_file_bb(c, s);
        Bitboard targets = 0;
        while(pushes) {
            Square to = pop_lsb(&pushes);
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
