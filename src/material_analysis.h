#ifndef MATERIAL_ANALYSIS_H_INCLUDED
#define MATERIAL_ANALYSIS_H_INCLUDED

#include "board.h"            // Định nghĩa S_BOARD
#include "material_entry.h"   // Định nghĩa MaterialEntry
#include "evaluation_types.h" // Định nghĩa SF::Key, SimpleHashTable

namespace { // Hoặc scope khác
    // Kích thước bảng băm (Stockfish 11 dùng 8192)
    constexpr int MaterialHashTableSize = 8192;
    typedef SF::SimpleHashTable<MaterialEntry, MaterialHashTableSize> MaterialHashTable;
}

/**
 * @brief Truy xuất hoặc tính toán MaterialEntry cho thế cờ hiện tại từ bảng băm.
 *
 * Kiểm tra MaterialHashTable dựa trên materialKey được tính từ số lượng quân cờ.
 * Nếu không tìm thấy, gọi analyze_material để tính toán, lưu và trả về entry mới.
 *
 * @param pos Con trỏ tới cấu trúc S_BOARD.
 * @return Con trỏ tới MaterialEntry hợp lệ. Trả về nullptr nếu pos là null.
 */
MaterialEntry* probe_material_table(const S_BOARD* pos);

// (Phiên bản cho đa luồng nếu cần)
// MaterialEntry* probe_material_table(const S_BOARD* pos, MaterialHashTable& table);

/**
 * @brief Phân tích vật chất và điền kết quả vào MaterialEntry.
 *
 * Tính toán gamePhase, imbalance, và các hệ số điều chỉnh mặc định.
 * Xác định (nhưng chưa gán) các trường hợp tàn cuộc đặc biệt.
 * Được gọi bởi probe_material_table khi cache miss.
 *
 * @param pos Con trỏ tới cấu trúc S_BOARD.
 * @param entry Con trỏ tới MaterialEntry sẽ được điền kết quả.
 * @return Con trỏ tới entry đã được điền.
 */
MaterialEntry* analyze_material(const S_BOARD* pos, MaterialEntry* entry);

/**
 * @brief Tính toán khóa Zobrist chỉ dựa trên số lượng quân cờ.
 *
 * Hàm này cần được định nghĩa (ví dụ trong material_analysis.cpp)
 * để tạo key cho MaterialHashTable. Nó khác với posKey và pawnKey.
 *
 * @param pos Con trỏ tới cấu trúc S_BOARD.
 * @return Khóa Zobrist 64-bit đại diện cho cấu hình vật chất.
 */
SF::Key calculate_material_key(const S_BOARD* pos);

/**
 * @brief Tính tổng giá trị vật chất không tính Tốt cho một bên.
 *
 * @param pos Con trỏ tới cấu trúc S_BOARD.
 * @param c Màu cần tính.
 * @return Giá trị vật chất (MG) không tính Tốt.
 */
SF::Value non_pawn_material(const S_BOARD* pos, SF::Color c);


#endif // MATERIAL_ANALYSIS_H_INCLUDED