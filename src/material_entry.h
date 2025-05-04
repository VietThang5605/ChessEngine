#ifndef MATERIAL_ENTRY_H_INCLUDED
#define MATERIAL_ENTRY_H_INCLUDED

#include "evaluation_types.h" // Cần cho SF::Key, SF::Score, SF::Phase, SF::Color, SF::Value, SF::ScaleFactor

// Forward declaration cho các hàm đánh giá/scaling tàn cuộc (nếu bạn muốn dùng con trỏ hàm sau này)
// class Position; // Nếu dùng kiểu Position của Stockfish
// typedef SF::Value (*EvalEndgameFunc)(const Position&);
// typedef SF::ScaleFactor (*ScaleEndgameFunc)(const Position&);

/**
 * @struct MaterialEntry
 * @brief Lưu trữ kết quả phân tích vật chất cho một thế cờ.
 *
 * Bao gồm điểm mất cân bằng, pha ván cờ, hệ số điều chỉnh mặc định,
 * và (tùy chọn) con trỏ/định danh tới các hàm xử lý tàn cuộc đặc biệt.
 */
struct MaterialEntry {
    SF::Key key = 0; ///< Khóa Zobrist 64-bit chỉ dựa trên số lượng quân cờ.

    /**
     * @brief Điểm mất cân bằng vật chất (chỉ giá trị MG/EG giống nhau).
     * Giá trị dương lợi cho Trắng, âm lợi cho Đen.
     * Lưu dưới dạng int16_t để khớp với Stockfish 11 Material::Entry::value.
     */
    int16_t imbalance_value = 0;

    SF::Phase gamePhase = SF::PHASE_ENDGAME; ///< Pha ván cờ (0 = Endgame, 128 = Midgame).

    /**
     * @brief Hệ số điều chỉnh điểm số mặc định cho mỗi bên.
     * Được sử dụng khi không có hàm scaling tàn cuộc đặc biệt nào được áp dụng.
     * Giá trị từ 0 (hòa hoàn toàn) đến 64 (không điều chỉnh).
     */
    uint8_t factor[SF::COLOR_NB] = { SF::SCALE_FACTOR_NORMAL, SF::SCALE_FACTOR_NORMAL };

    // --- Tùy chọn: Con trỏ/Định danh cho hàm tàn cuộc đặc biệt ---
    // Nếu dùng con trỏ hàm:
    // EvalEndgameFunc evalFunc = nullptr;
    // ScaleEndgameFunc scaleFunc[SF::COLOR_NB] = { nullptr, nullptr };
    // Hoặc dùng enum/ID để xác định hàm cần gọi:
    // int specialEvalID = 0; // 0 = không có
    // int specialScaleID[SF::COLOR_NB] = { 0, 0 };

    // --- Hàm tiện ích ---
    /**
     * @brief Trả về điểm mất cân bằng dưới dạng SF::Score (MG và EG bằng nhau).
     */
    SF::Score imbalance() const {
        return SF::make_score(imbalance_value, imbalance_value);
    }

    /**
     * @brief Lấy hệ số điều chỉnh cho một bên.
     * (Sau này có thể mở rộng để gọi hàm scaleFunc nếu có).
     */
    SF::ScaleFactor scale_factor(SF::Color c /*, const S_Board* pos - cần nếu gọi hàm scaleFunc*/) const {
        // if (scaleFunc[c]) {
        //     // Gọi hàm scaleFunc đặc biệt (cần truyền pos)
        //     // return scaleFunc[c](pos); // Giả sử scaleFunc nhận S_Board
        // }
        // *** SỬA ĐỔI: Trả về giá trị factor đã tính ***
        return SF::ScaleFactor(factor[c]); // Chỉ dùng factor mặc định cho bước này
        // return SF::ScaleFactor(SF::SCALE_FACTOR_NORMAL); // Dòng này bị loại bỏ/comment
    }

     /**
      * @brief Kiểm tra xem có hàm đánh giá tàn cuộc đặc biệt không.
      */
    // bool specialized_eval_exists() const { return evalFunc != nullptr; }

     /**
      * @brief Gọi hàm đánh giá tàn cuộc đặc biệt.
      */
    // SF::Value evaluate(const S_Board* pos) const {
    //     // assert(specialized_eval_exists());
    //     // return evalFunc(pos); // Giả sử evalFunc nhận S_Board
    //      return SF::VALUE_NONE; // Placeholder
    // }
};

#endif // MATERIAL_ENTRY_H_INCLUDED