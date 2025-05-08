#ifndef PAWN_ANALYSIS_H_INCLUDED
#define PAWN_ANALYSIS_H_INCLUDED

#include "board.h"            
#include "pawn_entry.h"       
#include "eval_help.h" 

namespace { 
    // Kích thước bảng băm cho PawnEntry
    constexpr int PawnHashTableSize = 8192;
    // Định nghĩa kiểu hash table
    typedef eval_help::SimpleHashTable<PawnEntry, PawnHashTableSize> PawnHashTable;
}

// Truy xuất hoặc tính toán PawnEntry cho thế cờ hiện tại từ bảng băm.
PawnEntry* probe_pawn_table(const S_BOARD* pos);

// (Phiên bản cho đa luồng nếu cần)
// PawnEntry* probe_pawn_table(const S_BOARD* pos, PawnHashTable& table);

// Phân tích cấu trúc Tốt và điền kết quả vào PawnEntry.
PawnEntry* analyze_pawns(const S_BOARD* pos, PawnEntry* entry);

// eval_help::Score CalculateKingShelterAndStorm(const S_BOARD* pos, Color kingColor);

#endif // PAWN_ANALYSIS_H_INCLUDED
