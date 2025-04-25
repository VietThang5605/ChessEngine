#include "evaluation_types.h"

namespace SF {
    uint8_t SquareDistance[SQUARE_NB][SQUARE_NB];
    Bitboard SquareBB[SQUARE_NB];
    //uint8_t SquareDistance[SQUARE_NB][SQUARE_NB];

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


}
