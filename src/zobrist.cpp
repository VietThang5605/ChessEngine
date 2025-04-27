#include "zobrist.h"
#include <random> // Hoặc dùng PRNG như Stockfish
#include "types.h" // Để sử dụng enum

namespace Zobrist {
    SF::Key Psq[13][64];
    // Định nghĩa các key khác nếu có...

    void init() {
        std::mt19937_64 engine(1070372); // Seed ngẫu nhiên
        std::uniform_int_distribution<SF::Key> distribution;

        for (int pc = wP; pc <= bK; ++pc) { // Enum từ types.h
            // Chỉ cần tạo key cho Tốt để tính pawnKey
            if (SF::type_of(static_cast<SF::Piece>(pc)) == SF::PAWN) {
                for (int sq64 = 0; sq64 < 64; ++sq64) {
                    Psq[pc][sq64] = distribution(engine);
                    // Đảm bảo key khác 0 nếu PRNG có thể tạo 0
                    if (Psq[pc][sq64] == 0) Psq[pc][sq64] = distribution(engine);
                }
            } else {
                 // Có thể tạo key cho các quân khác nếu muốn tính posKey đầy đủ
                  for (int sq64 = 0; sq64 < 64; ++sq64) Psq[pc][sq64] = 0; // Ví dụ
            }
        }
        // Khởi tạo các key khác nếu cần (Enpassant, Castling, Side)...
    }
}