#ifndef EVAL_HELP_H
#define EVAL_HELP_H

#include <algorithm>
#include <cassert>
#include <vector>

#include "types.h"

#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace eval_help {

//-------------------- Giá trị tuyệt đối (không có MG/EG)---------------------------
    enum Value : int {
        VALUE_ZERO      = 0,
        VALUE_DRAW      = 0,
        // Các giá trị Mate/Infinite có thể cần nếu bạn xử lý điểm số đặc biệt
        VALUE_KNOWN_WIN = 10000,
        VALUE_MATE      = 32000,
        VALUE_INFINITE  = 32001,
        VALUE_NONE      = 32002,
    
    
        // Giá trị quân cờ (lấy từ Stockfish)
        PawnValueMg   = 128,   PawnValueEg   = 213,
        KnightValueMg = 781,   KnightValueEg = 854,
        BishopValueMg = 825,   BishopValueEg = 915,
        RookValueMg   = 1276,  RookValueEg   = 1380,
        QueenValueMg  = 2538,  QueenValueEg  = 2682,
    
        // Giới hạn để tính Phase (lấy từ Stockfish)
        MidgameLimit  = 15258, EndgameLimit  = 3915
    };

//--------------------Square_pawn---------------------------
    enum Square_pawn : int {
        SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
        SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
        SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
        SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
        SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
        SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
        SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
        SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
        SQ_NONE, // 64
    
        SQUARE_NB = 64
    };
    extern U64 SquareBB[SQUARE_NB];
    void init_U64_utils(); // Khai báo hàm khởi tạo các mảng U64 (định nghĩa trong eval_help.cpp)

//----------------------------- Score-------------------
    enum Score : int { SCORE_ZERO };

    constexpr Score make_score(int mg, int eg) {
    // Chuyển đổi int thành unsigned int trước khi dịch bit để tránh UB
    return Score((int)((unsigned int)eg << 16) + mg);
    }

    inline Value mg_value(Score s) {
    // Dùng union để trích xuất 16 bit thấp một cách an toàn
    union { uint16_t u; int16_t s; } mg = { uint16_t(unsigned(s)) };
    return Value(mg.s);
    }

    inline Value eg_value(Score s) {
    // Dùng union để trích xuất 16 bit cao một cách an toàn
    union { uint16_t u; int16_t s; } eg = { uint16_t(unsigned(s + 0x8000) >> 16) };
    return Value(eg.s);
    }

    // Các toán tử cho Score (lấy từ Stockfish)
    #define ENABLE_BASE_OPERATORS_ON(T)                                \
    constexpr T operator+(T d1, T d2) { return T(int(d1) + int(d2)); } \
    constexpr T operator-(T d1, T d2) { return T(int(d1) - int(d2)); } \
    constexpr T operator-(T d) { return T(-int(d)); }                  \
    inline T& operator+=(T& d1, T d2) { return d1 = d1 + d2; }         \
    inline T& operator-=(T& d1, T d2) { return d1 = d1 - d2; }

    #define ENABLE_INCR_OPERATORS_ON(T)                                \
    inline T& operator++(T& d) { return d = T(int(d) + 1); }           \
    inline T& operator--(T& d) { return d = T(int(d) - 1); }

    #define ENABLE_FULL_OPERATORS_ON(T)                                \
    ENABLE_BASE_OPERATORS_ON(T)                                        \
    constexpr T operator*(int i, T d) { return T(i * int(d)); }        \
    constexpr T operator*(T d, int i) { return T(int(d) * i); }        \
    constexpr T operator/(T d, int i) { return T(int(d) / i); }        \
    constexpr int operator/(T d1, T d2) { return int(d1) / int(d2); }  \
    inline T& operator*=(T& d, int i) { return d = T(int(d) * i); }    \
    inline T& operator/=(T& d, int i) { return d = T(int(d) / i); }

    ENABLE_BASE_OPERATORS_ON(Score)     // Áp dụng toán tử cơ bản cho Score

    // Chia Score cho một số nguyên
    inline Score operator/(Score s, int i) {
        if (i == 0) return SCORE_ZERO;
        return make_score(mg_value(s) / i, eg_value(s) / i);
    }
    
    // Nhân Score với một số nguyên
    inline Score operator*(Score s, int i) {
        Score result = make_score(mg_value(s) * i, eg_value(s) * i);
        // assert((i == 0) || (result / i) == s);
        return result;
    }
    
    // Nhân Score với boolean
    inline Score operator*(Score s, bool b) {
        return make_score(mg_value(s) * int(b), eg_value(s) * int(b));
    }


//-------- Hàm tìm, xóa LSB (Least Significant Bit) của một U64-------------------
    inline Square_pawn lsb(U64 b) {
        assert(b != 0);
    #if defined(_MSC_VER)
        unsigned long idx;
        #ifdef _WIN64
        _BitScanForward64(&idx, b);
        #else
        if (static_cast<uint32_t>(b)) { _BitScanForward(&idx, static_cast<uint32_t>(b)); }
        else { _BitScanForward(&idx, static_cast<uint32_t>(b >> 32)); idx += 32; }
        #endif
        return static_cast<Square_pawn>(idx);
    #elif defined(__GNUC__) || defined(__clang__)
        return static_cast<Square_pawn>(__builtin_ctzll(b));
    #else
        // Fallback chậm
        for(int i=0; i<64; ++i) { if((b>>i)&1) return Square_pawn(i); }
        return SQ_NONE;
    #endif
    }
    // Hàm tìm và xóa LSB
    inline Square_pawn pop_lsb(U64* b) {
        const Square_pawn s = lsb(*b);
        *b &= (*b - 1); // Xóa bit thấp nhất
        return s;
    }


//---------------------hash_table----------------------------
    template<class Entry, int Size>
    struct SimpleHashTable {
        static_assert(((Size - 1) & Size) == 0, "Hash table size must be a power of 2");

        Entry* operator[](U64 k) {
            // Dùng phép AND bit để lấy index trong phạm vi [0, Size-1]
            return &table[static_cast<uint32_t>(k) & (Size - 1)];
        }

        // Xóa toàn bộ bảng (gán U64 về 0 hoặc giá trị không hợp lệ)
        void clear() {
        // Duyệt qua từng entry trong bảng
        for (auto& entry : table) {
            // Giả định rằng Entry luôn có thành viên 'U64'
            // Gán U64 = 0 để đánh dấu entry là trống/không hợp lệ
            entry.U64 = 0;
        }
    }

        // Constructor khởi tạo vector với kích thước Size
        SimpleHashTable() : table(Size) {}

    private:
        std::vector<Entry> table; // Dùng vector cấp phát trên heap
    };


//---------------------Bitboard----------------------------
    // --- Các hằng số Bitboard ---
    constexpr U64 FileABB = 0x0101010101010101ULL;
    constexpr U64 FileBBB = FileABB << 1;
    constexpr U64 FileCBB = FileABB << 2;
    constexpr U64 FileDBB = FileABB << 3;
    constexpr U64 FileEBB = FileABB << 4;
    constexpr U64 FileFBB = FileABB << 5;
    constexpr U64 FileGBB = FileABB << 6;
    constexpr U64 FileHBB = FileABB << 7;
    constexpr U64 Rank1BB = 0xFFULL;
    constexpr U64 Rank2BB = Rank1BB << (8 * 1);
    constexpr U64 Rank3BB = Rank1BB << (8 * 2);
    constexpr U64 Rank4BB = Rank1BB << (8 * 3);
    constexpr U64 Rank5BB = Rank1BB << (8 * 4);
    constexpr U64 Rank6BB = Rank1BB << (8 * 5);
    constexpr U64 Rank7BB = Rank1BB << (8 * 6);
    constexpr U64 Rank8BB = Rank1BB << (8 * 7);
    constexpr U64 AllSquares = ~U64(0);

    constexpr bool is_ok(Square_pawn s) {
        return s >= SQ_A1 && s <= SQ_H8;
    }

    extern U64 Square_pawnBB[SQUARE_NB];
    
    inline Color color_of(Piece pc) {
      assert(pc >= wP && pc <= bK);
      return Color(pc >> 3);
    }
    
    constexpr File file_of(Square_pawn s) {
      return File(s & 7);
    }
    
    constexpr Rank rank_of(Square_pawn s) {
      return Rank(s >> 3);
    }

    constexpr Rank relative_rank(Color c, Rank r) {
        return Rank(r ^ (c * 7)); // WHITE (0) -> r, BLACK (1) -> 7-r
      }
      
    constexpr Rank relative_rank(Color c, Square_pawn s) {
        return relative_rank(c, rank_of(s));
    }

    // --- Các hàm tiện ích Bitboard ---

    inline U64 square_bb(Square_pawn s) { return is_ok(s) ? SquareBB[s] : U64(0); }

    // Hàm kiểm tra có nhiều hơn 1 bit 1
    inline bool more_than_one(U64 b) {
        return (b & (b - 1)) != 0;
    }
        // Hàm đếm số bit 1
    inline int popcount(U64 b) {
        #ifdef _MSC_VER
            #ifdef _WIN64
                // Cần #include <intrin.h> ở đầu file
                return static_cast<int>(_mm_popcnt_u64(b));
            #else
                // Cần #include <intrin.h> ở đầu file
                return static_cast<int>(__popcnt(static_cast<uint32_t>(b))) +
                    static_cast<int>(__popcnt(static_cast<uint32_t>(b >> 32)));
            #endif
        #elif defined(__GNUC__) || defined(__clang__)
            return __builtin_popcountll(b);
        #else
            int count = 0;
            while (b) { b &= (b - 1); count++; }
            return count;
        #endif
    }


    // === KHAI BÁO EXTERN CÁC HÀM TIỆN ÍCH KHÁC (ĐỊNH NGHĨA TRONG .CPP) ===
    extern U64 adjacent_files_bb(Square_pawn s);
    extern U64 rank_bb(Square_pawn s);
    extern U64 rank_bb(Rank r);
    extern U64 file_bb(File f);
    extern U64 file_bb(Square_pawn s);
    extern U64 forward_ranks_bb(Color c, Square_pawn s);
    extern U64 forward_file_bb(Color c, Square_pawn s);
    extern U64 passed_pawn_span(Color c, Square_pawn s);
    extern U64 pawn_attack_span(Color c, Square_pawn s);
    // ===================================================================

    // Các toán tử cho U64 và Square
    inline U64  operator&( U64  b, Square_pawn s) { return b &  square_bb(s); }
    inline U64  operator|( U64  b, Square_pawn s) { return b |  square_bb(s); }
    inline U64  operator^( U64  b, Square_pawn s) { return b ^  square_bb(s); }
    inline U64& operator|=(U64& b, Square_pawn s) { return b |= square_bb(s); }
    inline U64& operator^=(U64& b, Square_pawn s) { return b ^= square_bb(s); }
    inline U64  operator&(Square_pawn s, U64 b) { return b & s; }
    inline U64  operator|(Square_pawn s, U64 b) { return b | s; }
    inline U64  operator^(Square_pawn s, U64 b) { return b ^ s; }

    // Các toán tử cho File và Rank
    ENABLE_INCR_OPERATORS_ON(File)
    ENABLE_INCR_OPERATORS_ON(Rank)
    ENABLE_INCR_OPERATORS_ON(Square_pawn)
    

//---------------------Direction----------------------------
    enum Direction : int { NORTH = 8, EAST = 1, SOUTH = -NORTH, WEST = -EAST, NORTH_EAST = NORTH + EAST, SOUTH_EAST = SOUTH + EAST, SOUTH_WEST = SOUTH + WEST, NORTH_WEST = NORTH + WEST, NNE = 17, ENE = 10, ESE = -6, SSE = -15, SSW = -17, WSW = -10, WNW = 6, NNW = 15 };

    // Hàm dịch chuyển bitboard
    template<Direction D>
    constexpr U64 shift(U64 b) {
    // Đảm bảo các hằng số FileABB, FileHBB đã được định nghĩa ở trên
        return D == NORTH      ?  b << 8 :
            D == SOUTH      ?  b >> 8 :
            D == EAST       ? (b & ~FileHBB) << 1 :
            D == WEST       ? (b & ~FileABB) >> 1 :
            D == NORTH_EAST ? (b & ~FileHBB) << 9 :
            D == NORTH_WEST ? (b & ~FileABB) << 7 :
            D == SOUTH_EAST ? (b & ~FileHBB) >> 7 :
            D == SOUTH_WEST ? (b & ~FileABB) >> 9 :
            // Thêm các hướng khác nếu cần
            U64(0);
    }
}


#endif
