#include "board.h" // Hoặc hash.h
#include "evaluation_types.h"
#include "zobrist.h" // Cần để truy cập Zobrist::Psq

SF::Key calculate_initial_pawn_key(const S_Board* pos) {
    SF::Key finalKey = 0;
    // Đảm bảo Zobrist::init() đã được gọi trước đó!
    for (SF::Color c : { SF::WHITE, SF::BLACK }) {
        SF::Bitboard pawns = pos->pawnsBB[c];
        // Lấy enum Piece tương ứng từ types.h
        int vicePawnEnum = (c == SF::WHITE) ? wP : bP;
        while (pawns) {
            SF::Square s = SF::pop_lsb(&pawns);
            // Giả định Zobrist::Psq[vicePawnEnum][s] đã được khởi tạo
            finalKey ^= Zobrist::Psq[vicePawnEnum][s];
        }
    }
    return finalKey;
}