#ifndef ZOBRIST_H_INCLUDED
#define ZOBRIST_H_INCLUDED

#include "types.h"


namespace Zobrist {
    extern U64 Psq[13][64];
    extern U64 MaterialKeys[13][11];

    // Hàm khởi tạo các số Zobrist
    void init();
}
#endif // ZOBRIST_H_INCLUDED