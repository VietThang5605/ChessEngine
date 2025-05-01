#ifndef EVALUATION_TYPES_H_INCLUDED
#define EVALUATION_TYPES_H_INCLUDED

#include <cstdint>   // Cho uint64_t và các kiểu số nguyên khác
#include <algorithm> // Cho std::min, std::max
#include <cassert>   // Cho assert
#include <vector>   

// Include header cần thiết cho các built-in intrinsics (nếu dùng)
#if defined(_MSC_VER)
#include <intrin.h>
#endif

// Định nghĩa các kiểu cơ bản (lấy từ Stockfish)
// Lưu ý: Các enum này có thể trùng tên với enum trong types.h gốc (kiểu Vice),
// cần dùng namespace

namespace SF { // Đặt trong namespace SF (Stockfish) để tránh xung đột trực tiếp

  constexpr int MAX_MOVES = 256;
  constexpr int MAX_PLY   = 246;

//----------------enum-----------------
enum Color {
  WHITE, BLACK, COLOR_NB = 2
};
enum PieceType {
  NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
  ALL_PIECES = 0, // Thường không dùng trực tiếp trong đánh giá
  PIECE_TYPE_NB = 8
};
enum Piece {
  NO_PIECE,
  W_PAWN = 1, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING, // 1-6
  B_PAWN = 9, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING, // 9-14
  PIECE_NB = 16
};
enum Square : int {
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
enum File : int {
  FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB
};
enum Rank : int {
  RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB
};
enum Direction : int { NORTH = 8, EAST = 1, SOUTH = -NORTH, WEST = -EAST, NORTH_EAST = NORTH + EAST, SOUTH_EAST = SOUTH + EAST, SOUTH_WEST = SOUTH + WEST, NORTH_WEST = NORTH + WEST, NNE = 17, ENE = 10, ESE = -6, SSE = -15, SSW = -17, WSW = -10, WNW = 6, NNW = 15 };

enum ScaleFactor {
  SCALE_FACTOR_DRAW    = 0,
  SCALE_FACTOR_NORMAL  = 64,
  SCALE_FACTOR_MAX     = 128,
  SCALE_FACTOR_NONE    = 255
};


// --- Kiểu Key và Hash Table ---
typedef uint64_t Key;
typedef uint64_t Bitboard; // Đặt Bitboard ở đây cho nhất quán

// ---------------------Các hằng số và enum liên quan trực tiếp đến đánh giá
enum Phase {
  PHASE_ENDGAME,      // Giá trị 0
  PHASE_MIDGAME = 128, // Giá trị max của phase, dùng để nội suy
  MG = 0, EG = 1, PHASE_NB = 2 // Index cho mảng giá trị MG/EG
};
// Giá trị tuyệt đối (không có MG/EG)
enum Value : int {
  VALUE_ZERO      = 0,
  VALUE_DRAW      = 0,
  // Các giá trị Mate/Infinite có thể cần nếu bạn xử lý điểm số đặc biệt
  VALUE_KNOWN_WIN = 10000,
  VALUE_MATE      = 32000,
  VALUE_INFINITE  = 32001,
  VALUE_NONE      = 32002,

  VALUE_MATE_IN_MAX_PLY  =  VALUE_MATE - 2 * MAX_PLY,
  VALUE_MATED_IN_MAX_PLY = -VALUE_MATE + 2 * MAX_PLY,

  // ----------------------Giá trị quân cờ (lấy từ Stockfish)
  PawnValueMg   = 128,   PawnValueEg   = 213,
  KnightValueMg = 781,   KnightValueEg = 854,
  BishopValueMg = 825,   BishopValueEg = 915,
  RookValueMg   = 1276,  RookValueEg   = 1380,
  QueenValueMg  = 2538,  QueenValueEg  = 2682,

  // Giới hạn để tính Phase (lấy từ Stockfish)
  MidgameLimit  = 15258, EndgameLimit  = 3915
};

//---------------------hash_table----------------------------
typedef uint64_t Key;
template<class Entry, int Size>
struct SimpleHashTable {
    static_assert(((Size - 1) & Size) == 0, "Hash table size must be a power of 2");

    Entry* operator[](Key k) {
        // Dùng phép AND bit để lấy index trong phạm vi [0, Size-1]
        return &table[static_cast<uint32_t>(k) & (Size - 1)];
    }

    // Xóa toàn bộ bảng (gán key về 0 hoặc giá trị không hợp lệ)
    void clear() {
      // Duyệt qua từng entry trong bảng
      for (auto& entry : table) {
          // Giả định rằng Entry luôn có thành viên 'key'
          // Gán key = 0 để đánh dấu entry là trống/không hợp lệ
          entry.key = 0;
      }
  }

    // Constructor khởi tạo vector với kích thước Size
    SimpleHashTable() : table(Size) {}

private:
    std::vector<Entry> table; // Dùng vector cấp phát trên heap
};

//----------- Cấu trúc Score để lưu điểm MG và EG
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

// Áp dụng các macro
ENABLE_FULL_OPERATORS_ON(Value)
// ENABLE_FULL_OPERATORS_ON(Direction) // Direction không cần toán tử * /

ENABLE_INCR_OPERATORS_ON(PieceType)
ENABLE_INCR_OPERATORS_ON(Piece)     // Đã thêm cho Piece
ENABLE_INCR_OPERATORS_ON(Square)
ENABLE_INCR_OPERATORS_ON(File)
ENABLE_INCR_OPERATORS_ON(Rank)
// ENABLE_INCR_OPERATORS_ON(Direction) // Không cần ++/-- cho Direction

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


// Các hàm trợ giúp cơ bản (lấy từ Stockfish)

constexpr bool is_ok(Square s) {
    return s >= SQ_A1 && s <= SQ_H8;
}

constexpr PieceType type_of(Piece pc) {
  return PieceType(pc & 7);
}

inline Color color_of(Piece pc) {
  assert(pc >= W_PAWN && pc <= B_KING);
  return Color(pc >> 3);
}

constexpr File file_of(Square s) {
  return File(s & 7);
}

constexpr Rank rank_of(Square s) {
  return Rank(s >> 3);
}

constexpr Square flip_vertically(Square s) {
  return Square(s ^ 56); // XOR với 56 (binary 111000) - Lật A1<->A8
}

constexpr Square relative_square(Color c, Square s) {
  return Square(s ^ (c * 56)); // WHITE (0) -> s, BLACK (1) -> flip_vertically(s)
}

constexpr Rank relative_rank(Color c, Rank r) {
  return Rank(r ^ (c * 7)); // WHITE (0) -> r, BLACK (1) -> 7-r
}

constexpr Rank relative_rank(Color c, Square s) {
  return relative_rank(c, rank_of(s));
}

inline File map_to_queenside(File f) {
   return File(f <= FILE_D ? f : FILE_H - f); // Map A-H về A-D
}

constexpr Piece make_piece(Color c, PieceType pt) {
  return (pt != NO_PIECE_TYPE) ? Piece((c << 3) + pt) : NO_PIECE;
}

constexpr Square make_square(File f, Rank r) {
  return Square((r << 3) + f);
}

constexpr Piece operator~(Piece pc) {
  return (pc != NO_PIECE) ? Piece(pc ^ 8) : NO_PIECE; // Đảo màu quân
}

constexpr Color operator~(Color c) {
  return Color(c ^ BLACK); // Đảo màu
}


//********** Định nghĩa các kiểu dữ liệu và hàm hỗ trợ cho đánh giá **********/
// --- Các hằng số Bitboard ---
constexpr Bitboard FileABB = 0x0101010101010101ULL;
constexpr Bitboard FileBBB = FileABB << 1;
constexpr Bitboard FileCBB = FileABB << 2;
constexpr Bitboard FileDBB = FileABB << 3;
constexpr Bitboard FileEBB = FileABB << 4;
constexpr Bitboard FileFBB = FileABB << 5;
constexpr Bitboard FileGBB = FileABB << 6;
constexpr Bitboard FileHBB = FileABB << 7;
constexpr Bitboard Rank1BB = 0xFFULL;
constexpr Bitboard Rank2BB = Rank1BB << (8 * 1);
constexpr Bitboard Rank3BB = Rank1BB << (8 * 2);
constexpr Bitboard Rank4BB = Rank1BB << (8 * 3);
constexpr Bitboard Rank5BB = Rank1BB << (8 * 4);
constexpr Bitboard Rank6BB = Rank1BB << (8 * 5);
constexpr Bitboard Rank7BB = Rank1BB << (8 * 6);
constexpr Bitboard Rank8BB = Rank1BB << (8 * 7);
constexpr Bitboard AllSquares = ~Bitboard(0); // Thêm hằng số này
// Thêm các hằng số khác nếu cần (RankBB, Center, QueenSide,...)

// --- Các hàm tiện ích Bitboard (Khai báo) ---
// Mảng lưu khoảng cách (cần định nghĩa và khởi tạo ở .cpp)
extern uint8_t SquareDistance[SQUARE_NB][SQUARE_NB];
// Mảng lưu bitboard cho từng ô (cần định nghĩa và khởi tạo ở .cpp)
extern Bitboard SquareBB[SQUARE_NB];

// Hàm khởi tạo các mảng trên (định nghĩa ở .cpp)
void init_bitboard_utils();

// Hàm lấy khoảng cách
template<typename T1 = Square> inline int distance(Square x, Square y);
template<> inline int distance<Square>(Square x, Square y) { return SquareDistance[x][y]; }
template<> inline int distance<File>(Square x, Square y) { return std::abs(file_of(x) - file_of(y)); }
template<> inline int distance<Rank>(Square x, Square y) { return std::abs(rank_of(x) - rank_of(y)); }

// Hàm lấy bitboard của ô
inline Bitboard square_bb(Square s) { return is_ok(s) ? SquareBB[s] : Bitboard(0); }

// === ĐỊNH NGHĨA CÁC HÀM INLINE/CONSTEXPR TẠI ĐÂY ===

// Hàm dịch chuyển bitboard
template<Direction D>
constexpr Bitboard shift(Bitboard b) {
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
           Bitboard(0);
}

inline Bitboard shift(Bitboard b, Direction D) {
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
         Bitboard(0);
}

// Hàm tìm bit thấp nhất
inline Square lsb(Bitboard b) {
    assert(b != 0);
#if defined(_MSC_VER)
    unsigned long idx;
    #ifdef _WIN64
    _BitScanForward64(&idx, b);
    #else
    if (static_cast<uint32_t>(b)) { _BitScanForward(&idx, static_cast<uint32_t>(b)); }
    else { _BitScanForward(&idx, static_cast<uint32_t>(b >> 32)); idx += 32; }
    #endif
    return static_cast<Square>(idx);
#elif defined(__GNUC__) || defined(__clang__)
    return static_cast<Square>(__builtin_ctzll(b));
#else
    // Fallback chậm
    for(int i=0; i<64; ++i) { if((b>>i)&1) return Square(i); }
    return SQ_NONE;
#endif
}

// Hàm tìm và xóa LSB
inline Square pop_lsb(Bitboard* b) {
    const Square s = lsb(*b);
    *b &= (*b - 1); // Xóa bit thấp nhất
    return s;
}

// --- Hàm tìm bit cao nhất (MSB - Most Significant Bit) ---
// Trả về index (0-63) của bit 1 cao nhất.
// Trả về giá trị không xác định nếu b = 0.
inline Square msb(Bitboard b) {
  assert(b != 0); // Đảm bảo bitboard không rỗng
#if defined(_MSC_VER) // Trình biên dịch Microsoft Visual C++
  unsigned long idx;
  #ifdef _WIN64 // Phiên bản 64-bit
  _BitScanReverse64(&idx, b);
  #else // Phiên bản 32-bit
  if (b >> 32) {
      _BitScanReverse(&idx, static_cast<unsigned long>(b >> 32));
      idx += 32;
  } else {
      _BitScanReverse(&idx, static_cast<unsigned long>(b));
  }
  #endif
  return static_cast<Square>(idx);
#elif defined(__GNUC__) || defined(__clang__) // Trình biên dịch GCC hoặc Clang
  // __builtin_clzll đếm số bit 0 ở đầu
  // Index MSB = 63 - số bit 0 ở đầu
  return static_cast<Square>(63 - __builtin_clzll(b));
#else
  // Fallback nếu không có intrinsics (chậm hơn)
  Square idx = SQ_H8; // Bắt đầu từ ô cao nhất
  Bitboard checkBit = Bitboard(1) << 63;
  while (checkBit) {
      if (b & checkBit) return idx;
      checkBit >>= 1;
      idx = static_cast<Square>(idx - 1);
  }
  return SQ_NONE; // Không nên xảy ra nếu b != 0
#endif
}

// --- Hàm tìm ô Tốt ở hàng trước nhất ---
// Trả về ô của Tốt ở hàng cao nhất (cho Trắng) hoặc thấp nhất (cho Đen)
inline Square frontmost_sq(Color c, Bitboard b) {
  assert(b != 0); // Đảm bảo bitboard không rỗng
  return (c == WHITE) ? msb(b) : lsb(b);
}

// Hàm đếm số bit 1
inline int popcount(Bitboard b) {
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

// Hàm kiểm tra có nhiều hơn 1 bit 1
inline bool more_than_one(Bitboard b) {
    return (b & (b - 1)) != 0;
}

// === KHAI BÁO EXTERN CÁC HÀM TIỆN ÍCH KHÁC (ĐỊNH NGHĨA TRONG .CPP) ===
extern Bitboard adjacent_files_bb(Square s);
extern Bitboard rank_bb(Square s);
extern Bitboard rank_bb(Rank r);
extern Bitboard file_bb(File f);
extern Bitboard file_bb(Square s);
extern Bitboard forward_ranks_bb(Color c, Square s);
extern Bitboard forward_file_bb(Color c, Square s);
extern Bitboard passed_pawn_span(Color c, Square s);
extern Bitboard pawn_attack_span(Color c, Square s);
// ===================================================================

// Các toán tử cho Bitboard và Square
inline Bitboard  operator&( Bitboard  b, Square s) { return b &  square_bb(s); }
inline Bitboard  operator|( Bitboard  b, Square s) { return b |  square_bb(s); }
inline Bitboard  operator^( Bitboard  b, Square s) { return b ^  square_bb(s); }
inline Bitboard& operator|=(Bitboard& b, Square s) { return b |= square_bb(s); }
inline Bitboard& operator^=(Bitboard& b, Square s) { return b ^= square_bb(s); }
inline Bitboard  operator&(Square s, Bitboard b) { return b & s; }
inline Bitboard  operator|(Square s, Bitboard b) { return b | s; }
inline Bitboard  operator^(Square s, Bitboard b) { return b ^ s; }


// *** Đặt các #undef ở cuối cùng, trước khi kết thúc namespace ***
#undef ENABLE_FULL_OPERATORS_ON
#undef ENABLE_INCR_OPERATORS_ON
#undef ENABLE_BASE_OPERATORS_ON

} // end namespace SF

#endif // #ifndef EVALUATION_TYPES_H_INCLUDED