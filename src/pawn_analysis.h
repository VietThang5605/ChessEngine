#ifndef PAWN_ANALYSIS_H_INCLUDED
#define PAWN_ANALYSIS_H_INCLUDED

#include "board.h"            // Định nghĩa S_Board
#include "pawn_entry.h"       // Định nghĩa PawnEntry (đã đổi tên từ pawns.h)
#include "evaluation_types.h" // Định nghĩa SF::Key, SimpleHashTable

namespace { // Hoặc scope khác nếu dùng ở nhiều file
    // Kích thước bảng băm (nên là lũy thừa của 2)
    // Stockfish 11 dùng 131072. 8192 là khởi đầu nhỏ hơn.
    constexpr int PawnHashTableSize = 8192;
    // Định nghĩa kiểu hash table
    typedef SF::SimpleHashTable<PawnEntry, PawnHashTableSize> PawnHashTable;
}

/**
 * @brief Truy xuất hoặc tính toán PawnEntry cho thế cờ hiện tại từ bảng băm.
 *
 * Hàm này là điểm truy cập chính để lấy thông tin cấu trúc Tốt.
 * Nó kiểm tra PawnHashTable trước. Nếu tìm thấy entry hợp lệ (key khớp),
 * nó sẽ trả về entry đó. Nếu không, nó sẽ gọi analyze_pawns để tính toán,
 * lưu kết quả vào bảng băm và trả về entry mới.
 *
 * @param pos Con trỏ tới cấu trúc S_Board chứa thế cờ. Cần đảm bảo pos->pawnKey đã được tính đúng.
 * @return Con trỏ tới PawnEntry hợp lệ. Trả về nullptr nếu pos là null.
 */
PawnEntry* probe_pawn_table(const S_Board* pos);

// (Phiên bản cho đa luồng nếu cần)
// PawnEntry* probe_pawn_table(const S_Board* pos, PawnHashTable& table);

/**
 * @brief Phân tích cấu trúc Tốt và điền kết quả vào PawnEntry.
 *
 * Hàm này thực hiện việc tính toán chi tiết điểm cấu trúc Tốt, xác định Tốt thông,
 * tính toán các bitboard tấn công của Tốt, v.v. Nó được gọi bởi probe_pawn_table
 * khi không tìm thấy entry trong bảng băm.
 *
 * @param pos Con trỏ tới cấu trúc S_Board chứa thế cờ.
 * @param entry Con trỏ tới PawnEntry sẽ được điền kết quả. Entry này sẽ được reset trước khi tính toán.
 * @return Con trỏ tới entry đã được điền (chính là con trỏ entry đầu vào).
 */
PawnEntry* analyze_pawns(const S_Board* pos, PawnEntry* entry);

#endif // PAWN_ANALYSIS_H_INCLUDED
