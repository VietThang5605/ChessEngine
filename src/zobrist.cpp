#include "zobrist.h"
#include <random> 
#include "types.h" 

namespace Zobrist {
    U64 Psq[13][64];
    U64 MaterialKeys[13][11];

    void init() {
        std::mt19937_64 engine(0xFEEDC0DECAFEBABFULL); // Seed ngẫu nhiên
        std::uniform_int_distribution<U64> distribution;

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

        // Khởi tạo MaterialKeys ---
        for (int pc = wP; pc <= bK; ++pc) { 
            MaterialKeys[pc][0] = 0;
            for (int count = 1; count <= 10; ++count) { 
                MaterialKeys[pc][count] = distribution(engine);
                while (MaterialKeys[pc][count] == 0) {
                    MaterialKeys[pc][count] = distribution(engine);
                }
            }
       }
        // Gán MaterialKeys[EMPTY][0] = 0 (không cần key cho ô trống)
        MaterialKeys[EMPTY][0] = 0; 
    }
}