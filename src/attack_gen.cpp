#include "attack_gen.h"
#include "evaluation_types.h" // File chứa SF::...
#include "board.h"             // File chứa S_BOARD và U64 (SF::Bitboard)

#include <vector> // Cần thiết nếu dùng std::vector cho directions

#include<iostream>
#include "bitboards.h" // Cần thiết nếu dùng SF::Square, SF::Bitboard



namespace AttackGen {

// --- Định nghĩa các bảng tấn công toàn cục ---
SF::Bitboard PseudoAttacks_KNIGHT[SF::SQUARE_NB];
SF::Bitboard PseudoAttacks_KING[SF::SQUARE_NB];
SF::Bitboard PawnAttacks[SF::COLOR_NB][SF::SQUARE_NB];


const int KnightOffsets[8] = { SF::NNE, SF::ENE, SF::ESE, SF::SSE, SF::SSW, SF::WSW, SF::WNW, SF::NNW };
const SF::Direction BishopDirections[4] = { SF::NORTH_EAST, SF::SOUTH_EAST, SF::SOUTH_WEST, SF::NORTH_WEST };
const SF::Direction RookDirections[4]   = { SF::NORTH, SF::EAST, SF::SOUTH, SF::WEST };
const SF::Direction KingDirections[8]   = { SF::NORTH, SF::EAST, SF::SOUTH, SF::WEST, SF::NORTH_EAST, SF::SOUTH_EAST, SF::SOUTH_WEST, SF::NORTH_WEST };


// --- Hàm khởi tạo bảng tấn công ---
// Gọi hàm này một lần khi engine khởi động
void init_attack_tables() {
     for (int s_int = 0; s_int < SF::SQUARE_NB; ++s_int) {
        SF::Square s = static_cast<SF::Square>(s_int);
        PseudoAttacks_KNIGHT[s] = 0;
        PseudoAttacks_KING[s] = 0;
        PawnAttacks[SF::WHITE][s] = 0; // Khởi tạo bằng 0
        PawnAttacks[SF::BLACK][s] = 0; // Khởi tạo bằng 0

        // Knight attacks
        for (int offset : KnightOffsets) {
            SF::Square to = SF::Square(s + offset);
            if (SF::is_ok(to) && SF::distance<SF::Square>(s, to) < 3) {
                 PseudoAttacks_KNIGHT[s] |= SF::square_bb(to);
            }
        }

        // King attacks
        for (SF::Direction dir : KingDirections) {
            SF::Square to = static_cast<SF::Square>(s + dir);
            if (SF::is_ok(to) && SF::distance<SF::Square>(s, to) == 1) {
                PseudoAttacks_KING[s] |= SF::square_bb(to);
            }
        }

        // === KHỞI TẠO PAWNATTACKS ===
            // Tấn công Tốt trắng
        SF::Square to_ne = static_cast<SF::Square>(s + SF::NORTH_EAST);
        SF::Square to_nw = static_cast<SF::Square>(s + SF::NORTH_WEST);
        if (SF::is_ok(to_ne) && SF::file_of(to_ne) > SF::file_of(s)) // Kiểm tra tràn cột H
            PawnAttacks[SF::WHITE][s] |= SF::square_bb(to_ne);
        if (SF::is_ok(to_nw) && SF::file_of(to_nw) < SF::file_of(s)) // Kiểm tra tràn cột A
            PawnAttacks[SF::WHITE][s] |= SF::square_bb(to_nw);

            // Tấn công Tốt đen
        SF::Square to_se = static_cast<SF::Square>(s + SF::SOUTH_EAST);
        SF::Square to_sw = static_cast<SF::Square>(s + SF::SOUTH_WEST);
        if (SF::is_ok(to_se) && SF::file_of(to_se) > SF::file_of(s)) // Kiểm tra tràn cột H
            PawnAttacks[SF::BLACK][s] |= SF::square_bb(to_se);
        if (SF::is_ok(to_sw) && SF::file_of(to_sw) < SF::file_of(s)) // Kiểm tra tràn cột A
            PawnAttacks[SF::BLACK][s] |= SF::square_bb(to_sw);
    }
}

// --- Triển khai các hàm tính toán tấn công (sử dụng S_BOARD*) ---

SF::Bitboard attacks_from_pawns(const S_Board* pos, SF::Color side) {
    SF::Bitboard pawns = pos->pawnsBB[side]; // Lấy bitboard Tốt từ S_BOARD
    // if (side == SF::WHITE) {
    //     return SF::shift<SF::NORTH_WEST>(pawns) | SF::shift<SF::NORTH_EAST>(pawns);
    // } else { // BLACK
    //     return SF::shift<SF::SOUTH_WEST>(pawns) | SF::shift<SF::SOUTH_EAST>(pawns);
    // }
    SF::Bitboard attacks = 0;
        // Tối ưu hơn là dùng bảng tính sẵn
    while (pawns) {
        attacks |= PawnAttacks[side][SF::pop_lsb(&pawns)];
    }
    return attacks;
}

SF::Bitboard attacks_from_knights(const S_Board* pos, SF::Color side) {
    SF::Bitboard attacks = 0;
    SF::Bitboard knights = pos->knightsBB[side]; // Lấy bitboard Mã từ S_BOARD
    while (knights) {
        attacks |= PseudoAttacks_KNIGHT[SF::pop_lsb(&knights)];
    }
    return attacks;
}

SF::Bitboard attacks_from_king(const S_Board* pos, SF::Color side) {
    SF::Square kingSq = static_cast<SF::Square>(pos->KingSq[side]); // Lấy ô Vua từ S_BOARD
    return SF::is_ok(kingSq) ? PseudoAttacks_KING[kingSq] : 0;
}

// Hàm helper tính toán tấn công cho quân trượt
// Hàm helper tính toán tấn công cho quân trượt
// SF::Bitboard attacks_from_sliding(SF::Bitboard sliders, SF::Bitboard occupied, const SF::Direction steps[], int numSteps) {
//     SF::Bitboard attacks = 0;
//     SF::Bitboard b = sliders;
//     while (b) {
//         SF::Square from = SF::pop_lsb(&b);

//         if (from == SF::SQ_A1) {
//             std::cout << "  DEBUG SLIDING from A1:" << std::endl;
//             std::cout << "    Occupied passed in:" << std::endl;
//             PrintBitBoard(occupied);
//         }

//         for (int i = 0; i < numSteps; ++i) {
//             SF::Direction dir = steps[i];

//             if (from == SF::SQ_A1 && dir == SF::EAST) {
//                 std::cout << "    Checking Direction: EAST" << std::endl;
//             }

//             SF::Square to = static_cast<SF::Square>(from + dir);
//             while (SF::is_ok(to)) {
//                 bool is_occupied = (occupied & SF::square_bb(to));
//                 if (from == SF::SQ_A1 && dir == SF::EAST) {
//                     std::cout << "      Considering square 'to': " << static_cast<int>(to)
//                               << " (Occupied: " << is_occupied << ")" << std::endl;
//                 }

//                 // --- DEBUG TRƯỚC KHI THÊM Ô ---
//                 if (from == SF::SQ_A1 && dir == SF::EAST && to == SF::SQ_E1) {
//                     std::cout << "      DEBUG OR: attacks BEFORE |=" << std::endl;
//                     PrintBitBoard(attacks);
//                 }

//                 attacks |= SF::square_bb(to); // Thêm ô vào kết quả

//                 // --- DEBUG SAU KHI THÊM Ô ---
//                 if (from == SF::SQ_A1 && dir == SF::EAST && to == SF::SQ_E1) {
//                     std::cout << "      DEBUG OR: attacks AFTER |=" << std::endl;
//                     PrintBitBoard(attacks);
//                 }

//                 if (is_occupied) {
//                     if (from == SF::SQ_A1 && dir == SF::EAST) {
//                         std::cout << "      Breaking because occupied." << std::endl;
//                     }
//                     break;
//                 }
//                 to = static_cast<SF::Square>(to + dir);
//                 if (SF::distance<SF::Square>(to, static_cast<SF::Square>(to - dir)) != 1) break;
//             }
//         }
//     }

//     if (sliders == SF::square_bb(SF::SQ_A1)) {
//         std::cout << "  DEBUG SLIDING from A1 RETURN VALUE:" << std::endl;
//         PrintBitBoard(attacks);
//     }
//     return attacks;
// }


SF::Bitboard attacks_from_sliding(SF::Bitboard sliders, SF::Bitboard occupied, const SF::Direction steps[], int numSteps) {
    SF::Bitboard attacks = 0;
    SF::Bitboard b = sliders;
    while (b) {
        SF::Square from = SF::pop_lsb(&b);
        for (int i = 0; i < numSteps; ++i) {
            SF::Direction dir = steps[i];
            SF::Square to = static_cast<SF::Square>(from + dir);
            while (SF::is_ok(to)) {
                 attacks |= SF::square_bb(to);
                 if (occupied & SF::square_bb(to)) {
                    std::cout << "  dsfhgbsdhfsdfghsdh" << std::endl;
                    PrintBitBoard(attacks);
                      break;
                 }
                 to = static_cast<SF::Square>(to + dir);
                 if (SF::distance<SF::Square>(to, static_cast<SF::Square>(to - dir)) != 1) break;
            }
        }
    }
    return attacks;
}

SF::Bitboard attacks_from_bishops(const S_Board* pos, SF::Color side) {
    SF::Bitboard bishops = pos->bishopsBB[side];
    // Cần bitboard chứa tất cả quân cờ
    SF::Bitboard occupied = pos->allPiecesBB[2];
    return attacks_from_sliding(bishops, occupied, BishopDirections, 4);
}

SF::Bitboard attacks_from_rooks(const S_Board* pos, SF::Color side) {
    SF::Bitboard rooks = pos->rooksBB[side];
    SF::Bitboard occupied = pos->allPiecesBB[2];
    return attacks_from_sliding(rooks, occupied, RookDirections, 4);
}

SF::Bitboard attacks_from_queens(const S_Board* pos, SF::Color side) {
    SF::Bitboard queens = pos->queensBB[side];
    SF::Bitboard occupied = pos->allPiecesBB[2];
    // Tấn công của Hậu = Tấn công Tượng + Tấn công Xe
    return attacks_from_sliding(queens, occupied, BishopDirections, 4)
         | attacks_from_sliding(queens, occupied, RookDirections, 4);
}


// (Tùy chọn) Hàm tính tổng tấn công cho một bên
SF::Bitboard attacks_by_side(const S_Board* pos, SF::Color side) {
    SF::Bitboard attacks = 0;
    attacks |= attacks_from_pawns(pos, side);
    attacks |= attacks_from_knights(pos, side);
    attacks |= attacks_from_bishops(pos, side);
    attacks |= attacks_from_rooks(pos, side);
    attacks |= attacks_from_queens(pos, side);
    attacks |= attacks_from_king(pos, side);
    return attacks;
}


} // namespace AttackGen