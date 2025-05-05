#include "pawn_analysis.h"
#include "evaluation_types.h" // Header chứa namespace SF và các kiểu dữ liệu
#include "attack_gen.h"       // Header chứa AttackGen::attacks_from_pawns và AttackGen::PawnAttacks
#include "bitboards.h"        // Nếu dùng PrintBitBoard để debug
#include "types.h"            // Cần cho enum Piece (wP, bP) và U64
#include "init.h"

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
SF::Score evaluate_structure(const S_BOARD* pos, PawnEntry* entry) {
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
PawnEntry* probe_pawn_table(const S_BOARD* pos) {
    assert(pos != nullptr);

    SF::Key key = pos->pawnKey; // Lấy pawnKey từ S_BOARD

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
    entry->scores[SF::WHITE] = entry->scores[SF::BLACK] = SF::SCORE_ZERO; // Dùng SF::
    entry->passedPawns[SF::WHITE] = entry->passedPawns[SF::BLACK] = 0;
    entry->pawnAttacks[SF::WHITE] = entry->pawnAttacks[SF::BLACK] = 0;
    entry->pawnAttacksSpan[SF::WHITE] = entry->pawnAttacksSpan[SF::BLACK] = 0;

    entry->kingSafety[SF::WHITE] = CalculateKingShelterAndStorm(pos, SF::WHITE);
    entry->kingSafety[SF::BLACK] = CalculateKingShelterAndStorm(pos, SF::BLACK);

    int kingSqW = pos->kingSquare[SF::WHITE];
    int kingSqB = pos->kingSquare[SF::BLACK];
    entry->kingSquares[SF::WHITE] = (kingSqW >= 0 && kingSqW < BRD_SQ_NUM) ? static_cast<SF::Square>(SQ64(kingSqW)) : SF::SQ_NONE;
    entry->kingSquares[SF::BLACK] = (kingSqB >= 0 && kingSqB < BRD_SQ_NUM) ? static_cast<SF::Square>(SQ64(kingSqB)) : SF::SQ_NONE;
    entry->castlingRights[SF::WHITE] = pos->castlePerm & (WKCA | WQCA); // Giả sử WKCA=1, WQCA=2
    entry->castlingRights[SF::BLACK] = pos->castlePerm & (BKCA | BQCA); // Giả sử BKCA=4, BQCA=8

    // Calculate structure scores and fill bitboards for both sides
    entry->scores[SF::WHITE] = evaluate_structure<SF::WHITE>(pos, entry); // Dùng SF::
    entry->scores[SF::BLACK] = evaluate_structure<SF::BLACK>(pos, entry); // Dùng SF::

    return entry;
}






// --- Hằng số cho King Safety (Lấy từ Stockfish 11 pawns.cpp) ---
namespace EvaluationConstants {
    #define V SF::Value
    #define S(mg, eg) SF::make_score(mg, eg)

    // Strength of pawn shelter for our king by [distance from edge][rank].
    const V ShelterStrength[4][SF::RANK_NB] = {
        { V( -6), V( 81), V( 93), V( 58), V( 39), V( 18), V(  25), V(0) }, // File A/H (dist 0)
        { V(-43), V( 61), V( 35), V(-49), V(-29), V(-11), V( -63), V(0) }, // File B/G (dist 1)
        { V(-10), V( 75), V( 23), V( -2), V( 32), V(  3), V( -45), V(0) }, // File C/F (dist 2)
        { V(-39), V(-13), V(-29), V(-52), V(-48), V(-67), V(-166),V(0) }  // File D/E (dist 3)
    };

    // Danger of enemy pawns moving toward our king by [distance from edge][rank].
    const V UnblockedStorm[4][SF::RANK_NB] = {
        { V( 85), V(-289), V(-166), V(97), V(50), V( 45), V( 50), V(0) }, // File A/H (dist 0)
        { V( 46), V( -25), V( 122), V(45), V(37), V(-10), V( 20), V(0) }, // File B/G (dist 1)
        { V( -6), V(  51), V( 168), V(34), V(-2), V(-22), V(-14), V(0) }, // File C/F (dist 2)
        { V(-15), V( -11), V( 101), V( 4), V(11), V(-15), V(-29), V(0) }  // File D/E (dist 3)
    };

    const SF::Score BlockedStorm  = S(82, 82);

    #undef V
    #undef S
}

SF::Score CalculateKingShelterAndStorm(const S_BOARD* pos, SF::Color kingColor) {
    int kingSq120 = pos->kingSquare[kingColor];
    int kingSq64 = Sq120ToSq64[kingSq120];
    if (kingSq64 < 0 || kingSq64 >= 64) return SF::SCORE_ZERO;

    SF::Square ksq = static_cast<SF::Square>(kingSq64);
    SF::Color enemyColor = ~kingColor;

    // Lấy Tốt của cả 2 bên không ở quá xa phía trước Vua
    SF::Bitboard ourPawns   = pos->pawnsBB[kingColor];
    SF::Bitboard theirPawns = pos->pawnsBB[enemyColor];
    // Bitboard relevantPawns = (ourPawns | theirPawns) & ~SF::forward_ranks_bb(enemyColor, ksq);
    // TODO: Cần hàm SF::forward_ranks_bb - Tạm thời lấy tất cả Tốt
    SF::Bitboard relevantOurPawns = ourPawns;
    SF::Bitboard relevantTheirPawns = theirPawns;


    SF::Score shelterScore = SF::SCORE_ZERO; // Bắt đầu với điểm cơ sở
    SF::File kingFile = SF::file_of(ksq);
    SF::File centerFile = std::max(SF::FILE_B, std::min(kingFile, SF::FILE_G)); // Giới hạn file vua trong B-G

    // Xét 3 cột: cột vua và 2 cột liền kề
    for (int f_int = centerFile - 1; f_int <= centerFile + 1; ++f_int) {
        SF::File f = static_cast<SF::File>(f_int);
        SF::Bitboard fileBB = SF::file_bb(f); // Cần hàm file_bb

        // Tìm Tốt của ta trên cột này
        SF::Bitboard ourPawnsOnFile = relevantOurPawns & fileBB;
        int ourPawnRank = 0; // Rank=0 nếu không có Tốt hoặc Tốt ở sau Vua
        if (ourPawnsOnFile) {
            SF::Square frontPawnSq = SF::frontmost_sq(kingColor, ourPawnsOnFile);
             // Chỉ tính nếu Tốt ở trước mặt Vua
             if(SF::relative_rank(kingColor, frontPawnSq) > SF::relative_rank(kingColor, ksq)) {
                  ourPawnRank = SF::relative_rank(kingColor, frontPawnSq);
             }
        }

        // Tìm Tốt của địch trên cột này
        SF::Bitboard theirPawnsOnFile = relevantTheirPawns & fileBB;
        int theirPawnRank = 0;
        if (theirPawnsOnFile) {
             SF::Square frontPawnSq = SF::frontmost_sq(kingColor, theirPawnsOnFile);
              if(SF::relative_rank(kingColor, frontPawnSq) > SF::relative_rank(kingColor, ksq)) {
                  theirPawnRank = SF::relative_rank(kingColor, frontPawnSq);
             }
        }

        // Tính khoảng cách từ mép bàn cờ (0=A/H, 1=B/G, 2=C/F, 3=D/E)
        int distFromEdge = std::min(static_cast<int>(f), static_cast<int>(SF::FILE_H) - static_cast<int>(f));

        // Cộng điểm che chắn từ Tốt của ta
        shelterScore += SF::make_score(EvaluationConstants::ShelterStrength[distFromEdge][ourPawnRank], 0);
            // std::cout << "che chắn từ Tốt của ta:111       " << shelterScore << std::endl;

        // Trừ điểm nguy hiểm từ Tốt địch
        // Nếu Tốt ta và Tốt địch đối đầu trực diện (cách nhau 1 rank)
        if (ourPawnRank > 0 && theirPawnRank > 0 && ourPawnRank == theirPawnRank - 1) {
            // Phạt nặng hơn nếu Tốt địch ở rank 3 của ta (rank 6 của địch nếu ta là Trắng)
             shelterScore -= EvaluationConstants::BlockedStorm * int(theirPawnRank == SF::RANK_6);
            //  std::cout << "che chắn từ Tốt của ta:222       " << shelterScore << std::endl;
        } else {
            // Phạt dựa trên vị trí Tốt địch không bị chặn
             shelterScore -= SF::make_score(EvaluationConstants::UnblockedStorm[distFromEdge][theirPawnRank], 0);
            //  std::cout << "che chắn từ Tốt của ta:3333        " << shelterScore << std::endl;
        }
    }

    // TODO: Xem xét điểm khi nhập thành (logic phức tạp hơn trong SF11 pawns.cpp)
    // if (pos->can_castle(kingColor & KING_SIDE)) { ... }
    // if (pos->can_castle(kingColor & QUEEN_SIDE)) { ... }

    // Phạt thêm ở EndGame nếu Vua ở xa Tốt của mình (khuyến khích Vua hoạt động)
     int minPawnDist = 8; // Giả sử xa nhất
     SF::Bitboard ourPawnsForDist = pos->pawnsBB[kingColor];
     if(ourPawnsForDist) {
         minPawnDist = 8;
         if (ourPawnsForDist & AttackGen::PseudoAttacks_KING[ksq]) { // Nếu Tốt ngay cạnh Vua
              minPawnDist = 1;
         } else {
              while(ourPawnsForDist) {
                  SF::Square psq = SF::pop_lsb(&ourPawnsForDist);
                  minPawnDist = std::min(minPawnDist, SF::distance<SF::Square>(ksq, psq));
              }
         }
     } else {
         minPawnDist = 0; // Không có Tốt thì không phạt
     }
    //  minPawnDist = 0; // Giới hạn khoảng cách tối đa
    shelterScore -= SF::make_score(0, 16 * minPawnDist); // Chỉ trừ vào EG
    // std::cout << "che chắn từ Tốt của ta:444      " << minPawnDist << std::endl;


    return shelterScore;
}