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
    PawnHashTable pawnTableInstance;

    // ---- Constants----
    #define S(mg, eg) eval_help::make_score(mg, eg)
    // constexpr eval_help::Score Isolated      = S(15, 20);
    constexpr eval_help::Score Isolated      = S(22, 28); // cũ: 15,20
    // constexpr eval_help::Score Backward      = S(10, 15);
    constexpr eval_help::Score Backward      = S(14, 18); // cũ: 10,15
    // constexpr eval_help::Score Doubled       = S(18, 30);
    constexpr eval_help::Score Doubled       = S(26, 36); // cũ: 18,30
    // constexpr eval_help::Score WeakUnopposed = S(8, 10);
    constexpr eval_help::Score WeakUnopposed = S(10, 12); // cũ: 8,10
    // constexpr eval_help::Score WeakLever     = S(0, 15);
    constexpr eval_help::Score WeakLever     = S(0, 18); // cũ: 0,15
    // constexpr int Connected[RANK_NB] = { 0, 4, 6, 10, 16, 25, 60 };
    constexpr int Connected[RANK_NB] = { 0, 6, 10, 16, 24, 36, 80 }; // cũ: 0,4,6,10,16,25,60
    // constexpr eval_help::Score PassedRank[RANK_NB] = { S(0 , 0), S(8, 15), S(16, 30), S(32, 50), S(50, 80), S(80, 120), S(130, 200) };
    constexpr eval_help::Score PassedRank[RANK_NB] = { S(0 , 0), S(12, 20), S(24, 40), S(48, 80), S(80, 130), S(130, 200), S(220, 320) }; // cũ: S(0,0),S(8,15),S(16,30),S(32,50),S(50,80),S(80,120),S(130,200)
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
    entry->pawnAttacks[Us] = AttackGen::attacks_from_pawns(pos, Us); 
    entry->pawnAttacksSpan[Us] = entry->pawnAttacks[Us];
    entry->passedPawns[Us] = 0;

    U64 theirPawnAttacks = AttackGen::attacks_from_pawns(pos, Them); 

    U64 currentPawns = ourPawns;

    U64 penalizedDoubledFiles = 0ULL;

    while (currentPawns) {
        eval_help::Square_pawn s = eval_help::pop_lsb(&currentPawns); 
        Rank r = eval_help::relative_rank(Us, eval_help::rank_of(s)); 
        File f = eval_help::file_of(s); 
        eval_help::Square_pawn pushSq = static_cast<eval_help::Square_pawn>(s + Up);
        U64 pushSq_bb = eval_help::square_bb(pushSq);

        // value for pawn structure
        int pst_value = 0;
        if (Us == WHITE) {
            pst_value = PawnTable[s];
        } else {
            pst_value = PawnTable[MIRROR64(s)];
        }

        // Features - Gọi các hàm tiện ích bitboard 
        
        U64 neighbours = ourPawns & eval_help::AdjacentFilesBB[s];
        U64 phalanx    = neighbours & eval_help::rank_bb(s);     
        U64 support    = neighbours & eval_help::rank_bb(static_cast<eval_help::Square_pawn>(s + Down));
        U64 opposed    = theirPawns & eval_help::ForwardFileBB[Us][s];
        U64 blocked    = theirPawns & pushSq_bb; // Chặn push           
        U64 stoppers   = theirPawns & eval_help::PassedPawnSpanBB[Us][s]; // Chặn tốt đã qua
        // Truy cập mảng PawnAttacks từ namespace AttackGen
        U64 lever      = theirPawns & AttackGen::PawnAttacks[Us][s]; 
        U64 leverPush  = 0;
        if (eval_help::is_ok(pushSq)) { 
             leverPush = theirPawns & AttackGen::PawnAttacks[Us][pushSq];
        }

        // bool doubled = bool(ourPawns & static_cast<eval_help::Square_pawn>(s + Down)); 

        // Backward pawn check
        bool backward = false;

        if (eval_help::is_ok(pushSq)) { 
            if (!support) {
                if (((theirPawnAttacks & pushSq_bb) || (theirPawns & pushSq_bb)) && !lever) {
                    backward = true;
                }
            }
        }

        // Update span
        if (!backward && !blocked) {
            entry->pawnAttacksSpan[Us] |= eval_help::PawnAttackSpanBB[Us][s]; 
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
            if (!backward && !passed && eval_help::more_than_one(lever)) { 
               score -= WeakLever;
            }
        }

        bool isPartOfDoubledStructure = (eval_help::popcount(ourPawns & eval_help::file_bb(s)) >= 2);
        if (isPartOfDoubledStructure && !support) {
            File f = eval_help::file_of(s);
            U64 fileMask = eval_help::file_bb(f);
            if (!(penalizedDoubledFiles & fileMask)) { // Kiểm tra xem cột này đã bị phạt chưa
                score -= Doubled;
                penalizedDoubledFiles |= fileMask; // Đánh dấu cột này đã bị phạt
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

    // Calculate structure scores and fill bitboards for both sides
    entry->scores[WHITE] = evaluate_structure<WHITE>(pos, entry);
    entry->scores[BLACK] = evaluate_structure<BLACK>(pos, entry);

    return entry;
}
