#include <algorithm>
#include <cassert> // Nên include nếu bạn dùng assert

#include "evaluation_types.h"
#include "psqt.h" // Định nghĩa PSQT::init() và các biến toàn cục

SF::Value PieceValue[SF::PHASE_NB][SF::PIECE_NB] = {
    { SF::VALUE_ZERO, SF::PawnValueMg, SF::KnightValueMg, SF::BishopValueMg, SF::RookValueMg, SF::QueenValueMg },
    { SF::VALUE_ZERO, SF::PawnValueEg, SF::KnightValueEg, SF::BishopValueEg, SF::RookValueEg, SF::QueenValueEg }
  };

namespace PSQT {

using namespace SF; 

#define S(mg, eg) make_score(mg, eg) // Macro tiện ích

// Định nghĩa các mảng Bonus và PBonus (Dùng const thay constexpr cho an toàn)
constexpr Score Bonus[SF::PIECE_TYPE_NB][SF::RANK_NB][SF::FILE_NB / 2] = {
  { }, { }, // NO_PIECE_TYPE, PAWN (không dùng Bonus trực tiếp cho Pawn)
  { // Knight 
   { S(-175, -96), S(-92,-65), S(-74,-49), S(-73,-21) },
   { S( -77, -67), S(-41,-54), S(-27,-18), S(-15,  8) },
   { S( -61, -40), S(-17,-27), S(  6, -8), S( 12, 29) },
   { S( -35, -35), S(  8, -2), S( 40, 13), S( 49, 28) },
   { S( -34, -45), S( 13,-16), S( 44,  9), S( 51, 39) },
   { S(  -9, -51), S( 22,-44), S( 58,-16), S( 53, 17) },
   { S( -67, -69), S(-27,-50), S(  4,-51), S( 37, 12) },
   { S(-201,-100), S(-83,-88), S(-56,-56), S(-26,-17) }
  },
  { // Bishop 
   { S(-53,-57), S( -5,-30), S( -8,-37), S(-23,-12) },
   { S(-15,-37), S(  8,-13), S( 19,-17), S(  4,  1) },
   { S( -7,-16), S( 21, -1), S( -5, -2), S( 17, 10) },
   { S( -5,-20), S( 11, -6), S( 25,  0), S( 39, 17) },
   { S(-12,-17), S( 29, -1), S( 22,-14), S( 31, 15) },
   { S(-16,-30), S(  6,  6), S(  1,  4), S( 11,  6) },
   { S(-17,-31), S(-14,-20), S(  5, -1), S(  0,  1) },
   { S(-48,-46), S(  1,-42), S(-14,-37), S(-23,-24) }
  },
  { // Rook 
   { S(-31, -9), S(-20,-13), S(-14,-10), S(-5, -9) },
   { S(-21,-12), S(-13, -9), S( -8, -1), S( 6, -2) },
   { S(-25,  6), S(-11, -8), S( -1, -2), S( 3, -6) },
   { S(-13, -6), S( -5,  1), S( -4, -9), S(-6,  7) },
   { S(-27, -5), S(-15,  8), S( -4,  7), S( 3, -6) },
   { S(-22,  6), S( -2,  1), S(  6, -7), S(12, 10) },
   { S( -2,  4), S( 12,  5), S( 16, 20), S(18, -5) },
   { S(-17, 18), S(-19,  0), S( -1, 19), S( 9, 13) }
  },
  { // Queen (INDEX = 5 = QUEEN)
   { S( 3,-69), S(-5,-57), S(-5,-47), S( 4,-26) },
   { S(-3,-55), S( 5,-31), S( 8,-22), S(12, -4) },
   { S(-3,-39), S( 6,-18), S(13, -9), S( 7,  3) },
   { S( 4,-23), S( 5, -3), S( 9, 13), S( 8, 24) },
   { S( 0,-29), S(14, -6), S(12,  9), S( 5, 21) },
   { S(-4,-38), S(10,-18), S( 6,-12), S( 8,  1) },
   { S(-5,-50), S( 6,-27), S(10,-24), S( 8, -8) },
   { S(-2,-75), S(-2,-52), S( 1,-43), S(-2,-36) }
  },
  { // King (INDEX = 6 = KING)
   { S(271,  1), S(327, 45), S(271, 85), S(198, 76) },
   { S(278, 53), S(303,100), S(234,133), S(179,135) },
   { S(195, 88), S(258,130), S(169,169), S(120,175) },
   { S(164,103), S(190,156), S(138,172), S( 98,172) },
   { S(154, 96), S(179,166), S(105,199), S( 70,199) },
   { S(123, 92), S(145,172), S( 81,184), S( 31,191) },
   { S( 88, 47), S(120,121), S( 65,116), S( 33,131) },
   { S( 59, 11), S( 89, 59), S( 45, 73), S( -1, 78) }
  }
  // Index 7 (ALL_PIECES) không dùng
};

constexpr Score PBonus[SF::RANK_NB][SF::FILE_NB] = { //pawn bonus
  { }, // Rank 1 (chỉ số 0) - không dùng
  { // Rank 2 (chỉ số 1)
    S(  3,-10), S(  3, -6), S( 10, 10), S( 19,  0), S( 16, 14), S( 19,  7), S(  7, -5), S( -5,-19) },
  { // Rank 3 (chỉ số 2)
    S( -9,-10), S(-15,-10), S( 11,-10), S( 15,  4), S( 32,  4), S( 22,  3), S(  5, -6), S(-22, -4) },
  { // Rank 4 (chỉ số 3)
    S( -8,  6), S(-23, -2), S(  6, -8), S( 20, -4), S( 40,-13), S( 17,-12), S(  4,-10), S(-12, -9) },
  { // Rank 5 (chỉ số 4)
    S( 13,  9), S(  0,  4), S(-13,  3), S(  1,-12), S( 11,-12), S( -2, -6), S(-13, 13), S(  5,  8) },
  { // Rank 6 (chỉ số 5)
    S( -5, 28), S(-12, 20), S( -7, 21), S( 22, 28), S( -8, 30), S( -5,  7), S(-15,  6), S(-18, 13) },
  { // Rank 7 (chỉ số 6)
    S( -7,  0), S(  7,-11), S( -3, 12), S(-13, 21), S(  5, 25), S(-16, 19), S( 10,  4), S( -8,  7) }
  // Rank 8 (chỉ số 7) - không dùng
};
#undef S

// Bảng PSQ cuối cùng (nên khai báo trong .h nếu cần truy cập từ bên ngoài, ví dụ: extern Score psq[...];)
Score psq[SF::PIECE_NB][SF::SQUARE_NB]; // PIECE_NB=16, SQUARE_NB=64

// Hàm khởi tạo bảng PSQ
void init() { // Giữ tên init() nếu bạn muốn giống Stockfish

    for (SF::Piece pc = W_PAWN; pc <= W_KING; ++pc)
    {
        // Sao chép giá trị vật chất từ quân trắng sang quân đen
        PieceValue[MG][~pc] = PieceValue[MG][pc];
        PieceValue[EG][~pc] = PieceValue[EG][pc];
    
        // Tính điểm vật chất cơ bản
        Score score = make_score(PieceValue[MG][pc], PieceValue[EG][pc]);
    
        // Duyệt qua các ô để tính điểm vị trí
        for (SF::Square s = SQ_A1; s <= SQ_H8; ++s)
        {
          SF::File f = file_of(s); // Lấy file thực tế
          SF::Rank r = rank_of(s); // Lấy rank thực tế
          SF::File qf = map_to_queenside(f); // Lấy file đối xứng bên cánh Hậu (A-D)
            
            // Tính điểm PSQ cho quân trắng
            // SỬA ĐỔI Ở ĐÂY: Dùng type_of(pc) để truy cập Bonus
            psq[ pc][ s] = score + (type_of(pc) == PAWN ? PBonus[r][f] // Dùng file thực 'f' cho Pawn
                                                        : Bonus[type_of(pc)][r][qf]); // Dùng type_of(pc) và file đối xứng 'qf'

            // Tính điểm PSQ cho quân đen (bằng cách lật ô và đảo dấu)
            SF::Square flipped_s = flip_vertically(s);
            if (is_ok(flipped_s)) { // Chỉ gán nếu ô lật hợp lệ
                 psq[~pc][flipped_s] = -psq[pc][s];
            }
        }
    }
}

} // namespace PSQT