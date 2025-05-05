
#include "material_analysis.h"
#include "evaluation_types.h"
#include "board.h"            // Cần S_BOARD, pceNum, Piece enums (wP, bK)
#include "types.h"            // Cần các enum Piece gốc của Vice (wP, bK)
#include "zobrist.h"          // Cần để tính material key nếu dùng Zobrist (mặc dù hàm hiện tại không dùng)

#include <cassert>
#include <cstring>   // Cho std::memset
#include <algorithm> // Cho std::min, std::max
#include <cmath>     // Cho std::round
#include <iostream>  // Cho debug prints

namespace {
    // --- Bảng băm toàn cục ---
    MaterialHashTable materialTableInstance;

    // --- Hằng số mất cân bằng vật chất (Giữ nguyên) ---
    constexpr int QuadraticOurs[][SF::PIECE_TYPE_NB] = {
        //            OUR PIECES
        // pair pawn knight bishop rook queen
        {1438                               }, // Bishop pair
        {  40,   38                         }, // Pawn
        {  32,  255, -62                    }, // Knight      OUR PIECES
        {   0,  104,   4,    0              }, // Bishop
        { -26,   -2,  47,   105,  -208      }, // Rook
        {-189,   24, 117,   133,  -134, -6  }  // Queen
      };
    constexpr int QuadraticTheirs[][SF::PIECE_TYPE_NB] = { 
        //           THEIR PIECES
        // pair pawn knight bishop rook queen
        {   0                               }, // Bishop pair
        {  36,    0                         }, // Pawn
        {   9,   63,   0                    }, // Knight      OUR PIECES
        {  59,   65,  42,     0             }, // Bishop
        {  46,   39,  24,   -24,    0       }, // Rook
        {  97,  100, -42,   137,  268,    0 }  // Queen
      };


    // --- *** HÀM HELPER MỚI: Ánh xạ Vice Enum sang SF::PieceType *** ---
    inline SF::PieceType GetPieceTypeFromViceEnum(int vicePieceEnum) {
        // Sử dụng enum từ types.h
        switch (vicePieceEnum) {
            case wP: case bP: return SF::PAWN;
            case wN: case bN: return SF::KNIGHT;
            case wB: case bB: return SF::BISHOP;
            case wR: case bR: return SF::ROOK;
            case wQ: case bQ: return SF::QUEEN;
            case wK: case bK: return SF::KING;
            default:          return SF::NO_PIECE_TYPE;
        }
    }
    // --- Kết thúc hàm helper ---


    // --- Hàm tính điểm mất cân bằng vật chất (Đã sửa lỗi tràn số) ---
     template<SF::Color Us>
     long long calculate_imbalance_bonus(const int pieceCount[][SF::PIECE_TYPE_NB]) { // *** Đổi kiểu trả về sang long long ***
        constexpr SF::Color Them = ~Us;
        long long bonus = 0; // *** Đổi sang long long ***

        //std::cout << "--- DEBUG IMBALANCE BONUS Color=" << static_cast<int>(Us) << " ---" << std::endl; // Tạm ẩn bớt

        // Xử lý cặp Tượng trước (index 0)
        if (pieceCount[Us][SF::NO_PIECE_TYPE]) {
             bonus += QuadraticOurs[SF::NO_PIECE_TYPE][SF::NO_PIECE_TYPE];
             for (int pt2_type = SF::NO_PIECE_TYPE; pt2_type <= SF::QUEEN; ++pt2_type) {
                  bonus += (long long)QuadraticTheirs[SF::NO_PIECE_TYPE][pt2_type] * pieceCount[Them][pt2_type]; // Ép kiểu lên long long khi nhân
             }
        }

        // Xử lý các quân còn lại (từ Tốt đến Hậu)
        for (int pt1_type = SF::PAWN; pt1_type <= SF::QUEEN; ++pt1_type)
        {
            if (pieceCount[Us][pt1_type]) {
                long long v = 0; // *** Đổi sang long long ***
                // Tương tác với cặp Tượng của Us (pt2 = 0)
                v += (long long)QuadraticOurs[pt1_type][SF::NO_PIECE_TYPE] * pieceCount[Us][SF::NO_PIECE_TYPE]; // Ép kiểu
                // Tương tác với các quân của Us từ Tốt đến pt1
                for (int pt2_type = SF::PAWN; pt2_type <= pt1_type; ++pt2_type) {
                     v += (long long)QuadraticOurs[pt1_type][pt2_type] * pieceCount[Us][pt2_type]; // Ép kiểu
                }
                // Tương tác với tất cả quân của địch (từ cặp Tượng đến Hậu)
                for (int pt2_type = SF::NO_PIECE_TYPE; pt2_type <= SF::QUEEN; ++pt2_type) {
                     v += (long long)QuadraticTheirs[pt1_type][pt2_type] * pieceCount[Them][pt2_type]; // Ép kiểu
                }
                bonus += (long long)pieceCount[Us][pt1_type] * v; // Ép kiểu
            }
        }
        //std::cout << " Final bonus for Color=" << static_cast<int>(Us) << " = " << bonus << std::endl; // Tạm ẩn bớt
        return bonus;
     }


} // end anonymous namespace


// // --- Hàm tính Material Key ---
// SF::Key calculate_material_key(const S_BOARD* pos) {
//      SF::Key key = 0;
//      // Tạo key dựa trên số lượng quân.
//      // Cần Zobrist keys riêng cho việc này (ví dụ: Zobrist::MaterialKey[piece_enum])
//      // Hoặc dùng tạm Zobrist::Psq[piece_enum][count] nếu đã khởi tạo đủ.
//      // Ví dụ đơn giản: XOR key của loại quân với số lượng quân đó.
//      // Đảm bảo Zobrist::init() đã khởi tạo các key cần thiết.
 
//      for (int pc = wP; pc <= bK; ++pc) { // Lặp qua các enum quân của Vice
//          if (pos->pieceNum[pc] > 0) {
//              // Lấy key cho loại quân pc (ví dụ dùng index 0 làm key loại quân)
//              SF::Key pieceTypeKey = Zobrist::Psq[pc][0];
//              if (pieceTypeKey != 0) {
//                  // XOR key này vào 'pos->pceNum[pc]' lần
//                  for (int i = 0; i < pos->pieceNum[pc]; ++i) {
//                       // XOR đơn giản cho mỗi quân. Một cách khác là có key cho (quân, số lượng).
//                       // Cách này có thể gây trùng lặp key (ví dụ 2 Mã và 1 Xe có thể trùng key với 1 Mã và 2 Xe nếu key Mã = key Xe).
//                       // Cần thiết kế hệ thống Zobrist key cẩn thận hơn cho material.
//                      key ^= pieceTypeKey;
//                  }
//              }
//          }
//      }
//      // Trả về key, đảm bảo không phải là 0 (vì 0 thường dùng cho entry trống)
//      return (key == 0) ? SF::Key(1) : key; // Trả về 1 nếu key tình cờ = 0
//  }
// --- HÀM calculate_material_key PHIÊN BẢN CUỐI CÙNG ---
SF::Key calculate_material_key(const S_BOARD* pos) {
    SF::Key key = 0;
    for (int pc = wP; pc <= bK; ++pc) { // Lặp qua các enum quân của Vice (wP=1 -> bK=12)
        int count = pos->pieceNum[pc]; // Lấy số lượng thực tế của quân pc

        // Chỉ XOR nếu có quân loại này (count > 0)
        if (count > 0) {
             // XOR trực tiếp với key tương ứng với loại quân (pc) và số lượng (count)
             // Cần đảm bảo count không vượt quá giới hạn của mảng MaterialKeys (đã định nghĩa là 10)
             if (count <= 10) {
                 key ^= Zobrist::MaterialKeys[pc][count];
             } else {
                 // Trường hợp lý thuyết số lượng quân > 10 (ví dụ 9 tốt phong Hậu?)
                 // Nên có xử lý an toàn ở đây, ví dụ báo lỗi hoặc dùng key[10]
                 assert(false && "Piece count exceeds material key limit for hashing");
                 // Hoặc dùng tạm key[10]: key ^= Zobrist::MaterialKeys[pc][10];
             }
        }
        // Không cần XOR gì nếu count = 0 (MaterialKeys[pc][0] nên bằng 0)
    }

    // Trả về key đã tính. Việc key = 0 là rất hiếm với hệ thống Zobrist tốt.
    // Có thể bỏ check == 0 nếu muốn.
    return (key == 0) ? SF::Key(1) : key; // Vẫn giữ lại để phòng trường hợp hy hữu
}






 // --- Probe function ---
// Tìm hoặc tính toán MaterialEntry trong hash table
MaterialEntry* probe_material_table(const S_BOARD* pos) {
     assert(pos != nullptr); // Đảm bảo con trỏ board hợp lệ
 
     // Tính key dựa trên số lượng quân (gọi hàm bạn đã định nghĩa)
     SF::Key key = calculate_material_key(pos);
 
     // Truy cập bảng băm toàn cục (materialTableInstance đã định nghĩa trong namespace ẩn danh)
     // Sử dụng operator[] của SimpleHashTable để lấy con trỏ tới entry tiềm năng
     MaterialEntry* entry = materialTableInstance[key];
 
     // Cache hit: Nếu key của entry khớp với key vừa tính và key khác 0
     if (entry->key == key && key != 0) {
         return entry; // Trả về entry đã tìm thấy trong cache
     }
 
     // Cache miss: Nếu không tìm thấy hoặc key không khớp (hoặc key=0)
     // Gọi hàm analyze_material để tính toán dữ liệu cho entry này
     // Hàm analyze_material (bạn đã có) sẽ điền dữ liệu vào 'entry'
     analyze_material(pos, entry);
 
     // Lưu key vừa tính vào entry mới để đánh dấu nó là hợp lệ cho lần probe sau
     entry->key = key;
 
     // Trả về con trỏ tới entry đã được tính toán và cập nhật
     return entry;
 }

// --- Hàm tính vật chất không phải Tốt ---
SF::Value non_pawn_material(const S_BOARD* pos, SF::Color c) {
     SF::Value totalValue = SF::VALUE_ZERO;
     int start_pc = (c == SF::WHITE) ? wN : bN; // Bắt đầu từ Mã
     int end_pc   = (c == SF::WHITE) ? wK : bK; // Kết thúc ở Vua
 
     // Giá trị vật chất tham khảo MG (lấy từ evaluation_types.h)
     const int pieceMaterialValueMg[13] = {
         0, 0 /*Pawn*/, SF::KnightValueMg, SF::BishopValueMg, SF::RookValueMg, SF::QueenValueMg, 0 /*King*/,
            0 /*Pawn*/, SF::KnightValueMg, SF::BishopValueMg, SF::RookValueMg, SF::QueenValueMg, 0 /*King*/
     };
 
     for (int pc = start_pc; pc <= end_pc; ++pc) {
         totalValue = static_cast<SF::Value>(static_cast<int>(totalValue) + pos->pieceNum[pc] * pieceMaterialValueMg[pc]);
     }
     return totalValue;
 }

// --- Main analysis function (Đã sửa lỗi tràn số) ---
MaterialEntry* analyze_material(const S_BOARD* pos, MaterialEntry* entry) {
    assert(pos != nullptr && entry != nullptr);

    // Reset entry
    entry->imbalance_value = 0;
    entry->gamePhase = SF::PHASE_ENDGAME;
    entry->factor[SF::WHITE] = entry->factor[SF::BLACK] = SF::SCALE_FACTOR_NORMAL;

    // --- Tính Game Phase (Giữ nguyên logic đã sửa) ---
    SF::Value npm_w = non_pawn_material(pos, SF::WHITE);
    SF::Value npm_b = non_pawn_material(pos, SF::BLACK);
    // std::cout << "DEBUG: npm_w=" << npm_w << " npm_b=" << npm_b << std::endl; // Debug npm
    double total_npm_precise = static_cast<double>(npm_w) + static_cast<double>(npm_b);
    //std::cout << "DEBUG: total_npm_precise (before clamp)=" << total_npm_precise << std::endl; // Tạm ẩn bớt
    total_npm_precise = std::min(total_npm_precise, static_cast<double>(SF::MidgameLimit));
    total_npm_precise = std::max(total_npm_precise, static_cast<double>(SF::EndgameLimit));
    //std::cout << "DEBUG: total_npm_precise (after clamp)=" << total_npm_precise << std::endl; // Tạm ẩn bớt

    double phase_precise = 0.0;
    if (SF::MidgameLimit > SF::EndgameLimit) {
        phase_precise = ((total_npm_precise - SF::EndgameLimit) * SF::PHASE_MIDGAME)
                             / (SF::MidgameLimit - SF::EndgameLimit);
    } else {
         phase_precise = (total_npm_precise >= SF::MidgameLimit) ? SF::PHASE_MIDGAME : SF::PHASE_ENDGAME;
    }
    int phase_int = static_cast<int>(std::round(phase_precise));
    //std::cout << "DEBUG: phase_precise=" << phase_precise << " phase_int=" << phase_int << std::endl; // Tạm ẩn bớt
    entry->gamePhase = static_cast<SF::Phase>(std::clamp(phase_int, 0, (int)SF::PHASE_MIDGAME));
    // std::cout << "DEBUG: final gamePhase=" << static_cast<int>(entry->gamePhase) << std::endl;


    // --- Tính điểm mất cân bằng vật chất (Đã sửa lỗi tràn số) ---
    int pieceCount[SF::COLOR_NB][SF::PIECE_TYPE_NB] = {{0}};
    for (int c_int = SF::WHITE; c_int <= SF::BLACK; ++c_int) {
        SF::Color c = static_cast<SF::Color>(c_int);
        int start_pc = (c == SF::WHITE) ? wP : bP;
        int end_pc   = (c == SF::WHITE) ? wK : bK;
        pieceCount[c][SF::NO_PIECE_TYPE] = (pos->pieceNum[(c == SF::WHITE) ? wB : bB] >= 2);
        for (int pc = start_pc; pc <= end_pc; ++pc) {
             SF::PieceType pt = GetPieceTypeFromViceEnum(pc);
             if (pt != SF::NO_PIECE_TYPE) {
                  pieceCount[c][pt] += pos->pieceNum[pc];
             }
        }
    }
     // In pieceCount để kiểm tra
    //  std::cout << "DEBUG: pieceCount[WHITE]: pair=" << pieceCount[SF::WHITE][0] << " P=" << pieceCount[SF::WHITE][1] << " N=" << pieceCount[SF::WHITE][2] << " B=" << pieceCount[SF::WHITE][3] << " R=" << pieceCount[SF::WHITE][4] << " Q=" << pieceCount[SF::WHITE][5] << " K=" << pieceCount[SF::WHITE][6] << std::endl;
    //  std::cout << "DEBUG: pieceCount[BLACK]: pair=" << pieceCount[SF::BLACK][0] << " P=" << pieceCount[SF::BLACK][1] << " N=" << pieceCount[SF::BLACK][2] << " B=" << pieceCount[SF::BLACK][3] << " R=" << pieceCount[SF::BLACK][4] << " Q=" << pieceCount[SF::BLACK][5] << " K=" << pieceCount[SF::BLACK][6] << std::endl;


    // Tính bonus bằng long long
    long long bonus_w_ll = calculate_imbalance_bonus<SF::WHITE>(pieceCount); // *** Gọi hàm trả về long long ***
    long long bonus_b_ll = calculate_imbalance_bonus<SF::BLACK>(pieceCount); // *** Gọi hàm trả về long long ***
    // std::cout << "DEBUG: bonus_w=" << bonus_w_ll << " bonus_b=" << bonus_b_ll << std::endl; // Debug bonus (long long)

    // Chia và làm tròn bằng double, sau đó ép kiểu về int16_t
    entry->imbalance_value = static_cast<int16_t>(std::round(static_cast<double>(bonus_w_ll - bonus_b_ll) / 16.0));
    // std::cout << "DEBUG: final imbalance_value=" << entry->imbalance_value << std::endl; // Debug giá trị cuối


    // --- Tính Scale Factor mặc định (Giữ nguyên logic SF11) ---
    int whitePawns = pos->pieceNum[wP];
    int blackPawns = pos->pieceNum[bP];
    SF::Value npm_diff_wb = npm_w - npm_b;
    SF::Value npm_diff_bw = npm_b - npm_w;
    if (whitePawns == 0 && npm_diff_wb <= SF::BishopValueMg) {
         entry->factor[SF::WHITE] = uint8_t(
             npm_w < SF::RookValueMg   ? SF::SCALE_FACTOR_DRAW
           : npm_b <= SF::BishopValueMg ? 4
           : 14
         );
    }
    if (blackPawns == 0 && npm_diff_bw <= SF::BishopValueMg) {
         entry->factor[SF::BLACK] = uint8_t(
             npm_b < SF::RookValueMg   ? SF::SCALE_FACTOR_DRAW
           : npm_w <= SF::BishopValueMg ? 4
           : 14
         );
    }

    return entry;
}