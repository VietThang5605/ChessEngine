#include "zobrist.h"
#include <random> // Hoặc dùng PRNG như Stockfish
#include "types.h" // Để sử dụng enum

namespace Zobrist {
    SF::Key Psq[13][64];
    SF::Key MaterialKeys[13][11];
    // Định nghĩa các key khác nếu có...

    void init() {
        std::mt19937_64 engine(1070372ULL); // Seed ngẫu nhiên
        std::uniform_int_distribution<SF::Key> distribution;

        // Khởi tạo Psq cho TẤT CẢ các loại quân trên tất cả các ô
        for (int pc = wP; pc <= bK; ++pc) { // Enum từ types.h (wP=1, bK=12)
            for (int sq64 = 0; sq64 < 64; ++sq64) {
                Psq[pc][sq64] = distribution(engine);
                // Đảm bảo key khác 0 nếu PRNG có thể tạo 0
                while (Psq[pc][sq64] == 0) {
                     Psq[pc][sq64] = distribution(engine);
                }
            }
        }
        // Gán Psq[EMPTY][sq] = 0 (không cần key cho ô trống)
        for (int sq64 = 0; sq64 < 64; ++sq64) {
            Psq[EMPTY][sq64] = 0;
        }
        // Khởi tạo các key khác nếu cần (Enpassant, Castling, Side)...

        // --- THÊM MỚI: Khởi tạo MaterialKeys ---
        for (int pc = wP; pc <= bK; ++pc) { // Lặp qua các loại quân wP -> bK
            // count = 0 không cần key (key = 0 là mặc định)
            MaterialKeys[pc][0] = 0;
            for (int count = 1; count <= 10; ++count) { // Số lượng từ 1 đến 10
                MaterialKeys[pc][count] = distribution(engine);
                while (MaterialKeys[pc][count] == 0) {
                    MaterialKeys[pc][count] = distribution(engine);
                }
            }
       }
       // Không cần key cho EMPTY trong MaterialKeys
        MaterialKeys[EMPTY][0] = 0; // Gán rõ ràng
    }
}