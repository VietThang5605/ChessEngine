#include "pawn_analysis.h"
#include "types.h" 
#include "eval_help.h"
#include "attack_gen.h"
#include "data.h"
#include "evaluate.h"

#include <cassert>
#include <cstring> 

#include "bitboards.h"//debug

// --- Hash table instance ---
namespace {
    // Bảng băm toàn cục (ví dụ cho đơn luồng)
    PawnHashTable pawnTableInstance; // PawnHashTable đã typedef trong pawn_analysis.h

    const double FactorMg = 100.0 / 128.0;
    const double FactorEg = 100.0 / 213.0;

    // --- Constants from Stockfish 11 pawns.cpp ---
    #define S(mg, eg) eval_help::make_score(mg, eg)
    constexpr eval_help::Score Isolated      = S( 5 * 100.0 / 128.0, 15 * 100.0 / 213.0);
    constexpr eval_help::Score Backward      = S( 9 * 100.0 / 128.0, 24 * 100.0 / 213.0);
    constexpr eval_help::Score Doubled       = S(11 * 100.0 / 128.0, 56 * 100.0 / 213.0);
    constexpr eval_help::Score WeakUnopposed = S(13 * 100.0 / 128.0, 27 * 100.0 / 213.0);
    constexpr eval_help::Score WeakLever     = S( 0, 56);
    constexpr int Connected[RANK_NB] = {
        0,
        int(7 * 100.0 / 128.0),
        int(8 * 100.0 / 128.0),
        int(12 * 100.0 / 128.0),
        int(29 * 100.0 / 128.0),
        int(48 * 100.0 / 128.0),
        int(86 * 100.0 / 128.0)
    };

    constexpr eval_help::Score PassedRank[RANK_NB] = {
        S(0 , 0), S(10 * 100.0 / 128.0, 28 *100.0 / 213.0), S(17 * 100.0 / 128.0, 33 *100.0 / 213.0), S(15 * 100.0 / 128.0, 41 *100.0 / 213.0), S(62 * 100.0 / 128.0, 72 *100.0 / 213.0), S(168 * 100.0 / 128.0, 177 *100.0 / 213.0), S(276 * 100.0 / 128.0, 260 *100.0 / 213.0)
        };

    //  // --- Bảng PBonus từ Stockfish (Điểm vị trí tốt) ---
    //  constexpr eval_help::Score PBonus[RANK_NB][FILE_NB] = 
    // { // Pawn (asymmetric distribution)
    //     { },
    //     { S(  3*100.0 / 128.0,-10 *100.0 / 213.0), S(  3*100.0 / 128.0, -6 *100.0 / 213.0), S( 10 *100.0 / 128.0, 10*100.0 / 213.0), S( 19 *100.0 / 128.0,  0), S( 16 *100.0 / 128.0, 14*100.0 / 213.0), S( 19 *100.0 / 128.0,  7*100.0 / 213.0), S(  7 *100.0 / 128.0, -5*100.0 / 213.0), S( -5 *100.0 / 128.0,-19*100.0 / 213.0) },
    //     { S( -9*100.0 / 128.0,-10*100.0 / 213.0), S(-15*100.0 / 128.0,-10*100.0 / 213.0), S( 11*100.0 / 128.0,-10*100.0 / 213.0), S( 15*100.0 / 128.0,  4*100.0 / 213.0), S( 32*100.0 / 128.0,  4*100.0 / 213.0), S( 22*100.0 / 128.0,  3*100.0 / 213.0), S(  5*100.0 / 128.0, -6*100.0 / 213.0), S(-22*100.0 / 128.0, -4*100.0 / 213.0) },
    //     { S( -8*100.0 / 128.0,  6*100.0 / 213.0), S(-23*100.0 / 128.0, -2*100.0 / 213.0), S(  6*100.0 / 128.0, -8*100.0 / 213.0), S( 20*100.0 / 128.0, -4*100.0 / 213.0), S( 40*100.0 / 128.0,-13*100.0 / 213.0), S( 17*100.0 / 128.0,-12*100.0 / 213.0), S(  4*100.0 / 128.0,-10*100.0 / 213.0), S(-12*100.0 / 128.0, -9*100.0 / 213.0) },
    //     { S( 13*100.0 / 128.0,  9*100.0 / 213.0), S(  0,  4*100.0 / 213.0), S(-13*100.0 / 128.0,  3*100.0 / 213.0), S(  1*100.0 / 128.0,-12*100.0 / 213.0), S( 11*100.0 / 128.0,-12*100.0 / 213.0), S( -2, -6*100.0 / 213.0), S(-13, 13*100.0 / 213.0), S(  5,  8*100.0 / 213.0) },
    //     { S( -5*100.0 / 128.0, 28*100.0 / 213.0), S(-12*100.0 / 128.0, 20*100.0 / 213.0), S( -7*100.0 / 128.0, 21*100.0 / 213.0), S( 22*100.0 / 128.0, 28*100.0 / 213.0), S( -8*100.0 / 128.0, 30*100.0 / 213.0), S( -5*100.0 / 128.0,  7*100.0 / 213.0), S(-15*100.0 / 128.0,  6*100.0 / 213.0), S(-18*100.0 / 128.0, 13*100.0 / 213.0) },
    //     { S( -7*100.0 / 128.0,  0), S(  7*100.0 / 128.0,-11*100.0 / 213.0), S( -3*100.0 / 128.0, 12*100.0 / 213.0), S(-13*100.0 / 128.0, 21*100.0 / 213.0), S(  5*100.0 / 128.0, 25*100.0 / 213.0), S(-16*100.0 / 128.0, 19*100.0 / 213.0), S( 10*100.0 / 128.0,  4*100.0 / 213.0), S( -8*100.0 / 128.0,  7*100.0 / 213.0) }
    // };
     #undef S

} // end anonymous namespace



// --- Helper function for pawn structure analysis ---
template<Color Us>
eval_help::Score evaluate_structure(const S_BOARD* pos, PawnEntry* entry) {
    
    constexpr Color Them = (Us == WHITE ? BLACK : WHITE);
    constexpr eval_help::Direction Up   = (Us == WHITE ? eval_help::NORTH : eval_help::SOUTH);
    constexpr eval_help::Direction Down = (Us == WHITE ? eval_help::SOUTH : eval_help::NORTH);

    U64 ourPawns   = pos->pawnsBB[Us];
    U64 theirPawns = pos->pawnsBB[Them];
    eval_help::Score score = eval_help::SCORE_ZERO;

    // Calculate pawn attacks and initialize span/passed
    entry->pawnAttacks[Us] = AttackGen::attacks_from_pawns(pos, Us); // Từ AttackGen
    entry->pawnAttacksSpan[Us] = entry->pawnAttacks[Us];
    entry->passedPawns[Us] = 0;

    U64 theirPawnAttacks = AttackGen::attacks_from_pawns(pos, Them); // Từ AttackGen

    U64 currentPawns = ourPawns;
    while (currentPawns) {
        eval_help::Square_pawn s = eval_help::pop_lsb(&currentPawns); 
        Rank r = eval_help::relative_rank(Us, eval_help::rank_of(s));
        File f = eval_help::file_of(s); 
        eval_help::Square_pawn pushSq = static_cast<eval_help::Square_pawn>(s + Up); 


        // ===========================================================
        // === THÊM ĐIỂM TỪ PBONUS VÀO ĐÂY ===
        // ===========================================================

        // Xác định ô cờ cần truy cập PBonus (mirror nếu là quân đen) bên trên có relative_rank
        // int sq64 = static_cast<int>(s);
        // int access_sq64 = (Us == WHITE) ? sq64 : MIRROR64(sq64); // MIRROR64 là hàm mirror ô cờ
        // Rank access_r = eval_help::rank_of(static_cast<eval_help::Square_pawn>(access_sq64));
        // File access_f = eval_help::file_of(static_cast<eval_help::Square_pawn>(access_sq64));

        // // Lấy điểm Score (MG, EG) từ PBonus
        // // Kiểm tra rank hợp lệ (thường là 1-6 tương ứng RANK_2 đến RANK_7)
        // if (access_r >= RANK_2 && access_r <= RANK_7) { // Các bảng PST thường chỉ định nghĩa cho các rank này
        //     assert(access_f >= FILE_A && access_f <= FILE_H);
        //     eval_help::Score bonus_score = PBonus[access_r][access_f];
            
        //      // Cộng điểm bonus vào tổng điểm cấu trúc
        //      score += bonus_score;
        // }
        // ===========================================================
        // === KẾT THÚC THÊM ĐIỂM PBONUS ===


        int pst_value = 0;
        if (Us == WHITE) {
            pst_value = PawnTable[s];
        } else {
            pst_value = PawnTable[MIRROR64(s)];
        }


        // ===========================================================
        // Features - Gọi các hàm tiện ích bitboard 
        U64 neighbours = ourPawns & eval_help::adjacent_files_bb(s);
        U64 phalanx    = neighbours & eval_help::rank_bb(s);       
        U64 support    = neighbours & eval_help::rank_bb(static_cast<eval_help::Square_pawn>(s + Down));
        U64 opposed    = theirPawns & eval_help::forward_file_bb(Us, s);    
        U64 blocked    = theirPawns & eval_help::square_bb(pushSq); // Chặn push
        U64 stoppers   = theirPawns & eval_help::passed_pawn_span(Us, s);
        // Truy cập mảng PawnAttacks từ namespace AttackGen
        U64 lever      = theirPawns & AttackGen::PawnAttacks[Us][s];//PrintBitBoard(lever);
        U64 leverPush  = 0;
        if (eval_help::is_ok(pushSq)) { 
             leverPush = theirPawns & AttackGen::PawnAttacks[Us][pushSq];
        }

        bool doubled = bool(ourPawns & static_cast<eval_help::Square_pawn>(s + Down)); 

        // Backward pawn check
        bool backward = false;

        if (eval_help::is_ok(pushSq)) { // Quan trọng: kiểm tra pushSq hợp lệ
            U64 pushSqBB = eval_help::square_bb(pushSq); // Chuyển sang bitboard
            if (!support) {
                if (((theirPawnAttacks & pushSqBB) || (theirPawns & pushSqBB)) && !lever) {
                    backward = true;
                }
            }
        }

        // Update span
        if (!backward && !blocked) {
            entry->pawnAttacksSpan[Us] |= eval_help::pawn_attack_span(Us, s);
        }

        // Passed pawn check
        bool passed = !(stoppers ^ lever);
        passed |= (!(stoppers ^ leverPush) && eval_help::popcount(phalanx) >= eval_help::popcount(leverPush)); 
        if (!passed && stoppers == blocked && r >= RANK_5) { 
             U64 supportAttackSq = eval_help::shift<Up>(support) & ~theirPawns & ~theirPawnAttacks; 
             passed |= bool(supportAttackSq);
        }

        if (passed) {
            entry->passedPawns[Us] |= eval_help::square_bb(s); 
        }

        // Calculate score
        if (support || phalanx) {
            int v = Connected[r] * (2 + bool(phalanx) - bool(opposed));
             score += eval_help::make_score(v, v * (r - 2) / 4); 
        } else if (!neighbours) {
            score -= Isolated + WeakUnopposed * !opposed;
        } else if (backward) {
             score -= Backward + WeakUnopposed * !opposed;
        }

        if(passed) {
            score += PassedRank[r]; // Điểm cho tốt đã qua
        }
        score += eval_help::make_score(pst_value, pst_value); // score for pawn structure in mg vs eg

        if (!support) {
            score -= Doubled * doubled;
            if (!backward && !passed && eval_help::more_than_one(lever)) { 
               score -= WeakLever;
            }
        }

    } // end while

    return score;
}





// --- Probe function ---
PawnEntry* probe_pawn_table(const S_BOARD* pos) {
    assert(pos != nullptr);

    U64 key = pos->pawnKey; // Lấy pawnKey từ S_BOARD

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
PawnEntry* analyze_pawns(const S_BOARD* pos, PawnEntry* entry) {
    assert(pos != nullptr && entry != nullptr);

    // Reset entry fields (không reset key)
    entry->scores[WHITE] = entry->scores[BLACK] = eval_help::SCORE_ZERO;  
    entry->passedPawns[WHITE] = entry->passedPawns[BLACK] = 0;
    entry->pawnAttacks[WHITE] = entry->pawnAttacks[BLACK] = 0;
    entry->pawnAttacksSpan[WHITE] = entry->pawnAttacksSpan[BLACK] = 0;

    // entry->kingSafety[WHITE] = CalculateKingShelterAndStorm(pos, WHITE);
    // entry->kingSafety[BLACK] = CalculateKingShelterAndStorm(pos, BLACK);

    // int kingSqW = pos->kingSquare[WHITE];
    // int kingSqB = pos->kingSquare[BLACK];
    // entry->kingSquares[WHITE] = (kingSqW >= 0 && kingSqW < BRD_SQ_NUM) ? static_cast<Square>(SQ64(kingSqW)) : SQ_NONE;
    // entry->kingSquares[BLACK] = (kingSqB >= 0 && kingSqB < BRD_SQ_NUM) ? static_cast<Square>(SQ64(kingSqB)) : SQ_NONE;
    // entry->castlingRights[WHITE] = pos->castlePerm & (WKCA | WQCA); // Giả sử WKCA=1, WQCA=2
    // entry->castlingRights[BLACK] = pos->castlePerm & (BKCA | BQCA); // Giả sử BKCA=4, BQCA=8

    // Calculate structure scores and fill bitboards for both sides
    entry->scores[WHITE] = evaluate_structure<WHITE>(pos, entry);
    entry->scores[BLACK] = evaluate_structure<BLACK>(pos, entry);

    return entry;
}