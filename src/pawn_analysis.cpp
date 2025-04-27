#include "pawn_analysis.h"
#include "evaluation_types.h" // Header chứa namespace SF và các kiểu dữ liệu
#include "attack_gen.h"       // Header chứa AttackGen::attacks_from_pawns và AttackGen::PawnAttacks
#include "bitboards.h"        // Nếu dùng PrintBitBoard để debug
#include "types.h"            // Cần cho enum Piece (wP, bP) và U64
#include <cassert>
#include <cstring>           // Cho std::memset

// --- Hash table instance ---
namespace {
    // Bảng băm toàn cục (ví dụ cho đơn luồng)
    PawnHashTable pawnTableInstance; // PawnHashTable đã typedef trong pawn_analysis.h

    // --- Constants from Stockfish 11 pawns.cpp ---
    #define S(mg, eg) SF::make_score(mg, eg)
    constexpr SF::Score Isolated      = S( 5, 15);
    constexpr SF::Score Backward      = S( 9, 24);
    constexpr SF::Score Doubled       = S(11, 56);
    constexpr SF::Score WeakUnopposed = S(13, 27);
    constexpr SF::Score WeakLever     = S( 0, 56);
    constexpr int Connected[SF::RANK_NB] = { 0, 7, 8, 12, 29, 48, 86 };
    #undef S

} // end anonymous namespace

// --- Helper function for pawn structure analysis ---
template<SF::Color Us>
SF::Score evaluate_structure(const S_Board* pos, PawnEntry* entry) {
    // Sử dụng tiền tố SF:: cho các kiểu và enum từ evaluation_types.h
    constexpr SF::Color Them = ~Us;
    constexpr SF::Direction Up   = (Us == SF::WHITE ? SF::NORTH : SF::SOUTH);
    constexpr SF::Direction Down = (Us == SF::WHITE ? SF::SOUTH : SF::NORTH);

    SF::Bitboard ourPawns   = pos->pawnsBB[Us];
    SF::Bitboard theirPawns = pos->pawnsBB[Them];
    SF::Score score = SF::SCORE_ZERO;

    // Calculate pawn attacks and initialize span/passed
    entry->pawnAttacks[Us] = AttackGen::attacks_from_pawns(pos, Us); // Từ AttackGen
    entry->pawnAttacksSpan[Us] = entry->pawnAttacks[Us];
    entry->passedPawns[Us] = 0;

    SF::Bitboard theirPawnAttacks = AttackGen::attacks_from_pawns(pos, Them); // Từ AttackGen

    SF::Bitboard currentPawns = ourPawns;
    while (currentPawns) {
        SF::Square s = SF::pop_lsb(&currentPawns); // Dùng SF::
        SF::Rank r = SF::relative_rank(Us, SF::rank_of(s)); // Dùng SF::
        SF::File f = SF::file_of(s); // Dùng SF::
        SF::Square pushSq = static_cast<SF::Square>(s + Up); // Dùng SF::





        
        // Features - Gọi các hàm tiện ích bitboard với tiền tố SF::
        SF::Bitboard neighbours = ourPawns & SF::adjacent_files_bb(s);
        SF::Bitboard phalanx    = neighbours & SF::rank_bb(s);
        SF::Bitboard support    = neighbours & SF::rank_bb(static_cast<SF::Square>(s + Down));
        SF::Bitboard opposed    = theirPawns & SF::forward_file_bb(Us, s);
        SF::Bitboard blocked    = theirPawns & pushSq;
        SF::Bitboard stoppers   = theirPawns & SF::passed_pawn_span(Us, s);
        // Truy cập mảng PawnAttacks từ namespace AttackGen
        SF::Bitboard lever      = theirPawns & AttackGen::PawnAttacks[Us][s];
        SF::Bitboard leverPush  = 0;
        if (SF::is_ok(pushSq)) { // Dùng SF::
             leverPush = theirPawns & AttackGen::PawnAttacks[Us][pushSq]; // Dùng AttackGen::
        }

        bool doubled = bool(ourPawns & static_cast<SF::Square>(s + Down)); // Dùng SF::

        // Backward pawn check
        bool backward = false;
        if (!(neighbours & SF::forward_ranks_bb(Us, s))) { // Dùng SF::
             if ((theirPawnAttacks & pushSq) || blocked) {
                 if (!blocked && !(theirPawnAttacks & pushSq) && !(pos->allPiecesBB[2] & pushSq) ) {
                 } else {
                     backward = true;
                 }
             }
        }

        // Update span
        if (!backward && !blocked) {
            entry->pawnAttacksSpan[Us] |= SF::pawn_attack_span(Us, s); // Dùng SF::
        }

        // Passed pawn check
        bool passed = !(stoppers ^ lever);
        passed |= (!(stoppers ^ leverPush) && SF::popcount(phalanx) >= SF::popcount(leverPush)); // Dùng SF::
        if (!passed && stoppers == blocked && r >= SF::RANK_5) { // Dùng SF::
             SF::Bitboard supportAttackSq = SF::shift<Up>(support) & ~theirPawns & ~theirPawnAttacks; // Dùng SF::
             passed |= bool(supportAttackSq);
        }

        if (passed) {
            entry->passedPawns[Us] |= SF::square_bb(s); // Dùng SF::
        }

        // Calculate score
        if (support || phalanx) {
            int v = Connected[r] * (2 + bool(phalanx) - bool(opposed));
             score += SF::make_score(v, v * (r - 2) / 4); // Dùng SF::
        } else if (!neighbours) {
            score -= Isolated + WeakUnopposed * !opposed;
        } else if (backward) {
             score -= Backward + WeakUnopposed * !opposed;
        }

        if (!support) {
            score -= Doubled * doubled;
            if (!backward && !passed && SF::more_than_one(lever)) { // Dùng SF::
               score -= WeakLever;
            }
        }

    } // end while

    return score;
}


// --- Probe function ---
PawnEntry* probe_pawn_table(const S_Board* pos) {
    assert(pos != nullptr);

    SF::Key key = pos->pawnKey; // Lấy pawnKey từ S_Board

    // Truy cập bảng băm (ví dụ đơn luồng)
    PawnEntry* entry = pawnTableInstance[key];

    // Kiểm tra key và trạng thái khởi tạo
    if (entry->key == key && key != 0) {
        return entry; // Cache hit
    }

    // Cache miss
    analyze_pawns(pos, entry); // Tính toán
    entry->key = key;          // Lưu key

    return entry;
}

// --- Main analysis function ---
PawnEntry* analyze_pawns(const S_Board* pos, PawnEntry* entry) {
    assert(pos != nullptr && entry != nullptr);

    // Reset entry fields (không reset key)
    entry->scores[SF::WHITE] = entry->scores[SF::BLACK] = SF::SCORE_ZERO; // Dùng SF::
    entry->passedPawns[SF::WHITE] = entry->passedPawns[SF::BLACK] = 0;
    entry->pawnAttacks[SF::WHITE] = entry->pawnAttacks[SF::BLACK] = 0;
    entry->pawnAttacksSpan[SF::WHITE] = entry->pawnAttacksSpan[SF::BLACK] = 0;

    // Calculate structure scores and fill bitboards for both sides
    entry->scores[SF::WHITE] = evaluate_structure<SF::WHITE>(pos, entry); // Dùng SF::
    entry->scores[SF::BLACK] = evaluate_structure<SF::BLACK>(pos, entry); // Dùng SF::

    return entry;
}
