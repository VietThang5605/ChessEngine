#include "evaluation.h"
#include "board.h"
#include "types.h"
#include "evaluation_types.h"
#include "eval_info.h"
#include "pawn_analysis.h"
#include "material_analysis.h"
#include "attack_gen.h"
#include "psqt.cpp"

#include <cassert>
#include <algorithm>
#include <cmath>
#include <vector>

// --- Định nghĩa các hằng số (Giữ nguyên như phiên bản trước, bổ sung thêm) ---
namespace EvaluationConstants {
    #define S(mg, eg) SF::make_score(mg, eg)

    // ... (Giữ nguyên KingAttackWeights, MobilityBonus, ThreatByMinor/Rook/King, PassedRank/File, SafeChecks, SpaceThreshold, Tempo) ...

    // *** BỔ SUNG HẰNG SỐ CHO PIECE EVALUATION ***
    const SF::Score BishopPawns        = S(  3,  7); // Penalty per pawn on same color square
    const SF::Score CorneredBishop     = S( 50, 50); // Penalty for trapped bishop (Chess960 specific in SF11)
    const SF::Score FlankAttacks       = S(  8,  0); // Penalty per attack count in king flank
    const SF::Score Hanging            = S( 69, 36); // Penalty for hanging piece
    const SF::Score KingProtector      = S(  7,  8); // Penalty for piece distance from king
    // const SF::Score KnightOnQueen      = S( 16, 12); // Bỏ qua
    const SF::Score LongDiagonalBishop = S( 45,  0); // Bonus for bishop on long diagonal controlling center
    const SF::Score MinorBehindPawn    = S( 18,  3); // Bonus for minor piece behind pawn
    const SF::Score Outpost            = S( 30, 21); // Bonus for piece on outpost square (N=x2, B=x1)
    const SF::Score PassedFile         = S( 11,  8); // Penalty for passed pawn file (map_to_queenside)
    const SF::Score PawnlessFlank      = S( 17, 95); // Penalty for king on flank with no friendly pawns
    const SF::Score RestrictedPiece    = S(  7,  7); // Bonus for attacking restricted enemy piece
    const SF::Score ReachableOutpost   = S( 32, 10); // Bonus for Knight attacking outpost square
    const SF::Score RookOnFile[]       = { S(21, 4), S(47, 25) }; // Bonus for rook on [semiopen, open] file
    const SF::Score RookOnQueenFile    = S(  7,  6); // Bonus for rook on same file as any queen
    // const SF::Score SliderOnQueen      = S( 59, 18); // Bỏ qua
    const SF::Score ThreatByKing       = S( 24, 89);
    const SF::Score ThreatByPawnPush   = S( 48, 39); // Cần logic phức tạp
    const SF::Score ThreatBySafePawn   = S(173, 94); // Cần logic phức tạp
    const SF::Score TrappedRook        = S( 52, 10); // Penalty for rook trapped by king
    // const SF::Score WeakQueen          = S( 49, 15); // Bỏ qua

    #undef S
}

// --- Helper Ánh xạ Enum Vice -> SF::PieceType (Giữ nguyên) ---
static inline SF::PieceType GetPieceTypeFromViceEnum(int vicePieceEnum) { /* ... */ }

// --- Hàm tính điểm PSQT (Giữ nguyên) ---
static SF::Score CalculatePSQT(const S_Board* pos) { /* ... */ }


// --- Hàm đánh giá chính (CẬP NHẬT) ---
SF::Value EvaluatePosition(const S_Board* pos) {
    assert(pos != nullptr);
    // TODO: Thêm kiểm tra thế cờ hợp lệ

    // 1. Lấy MaterialEntry và PawnEntry
    MaterialEntry* me = probe_material_table(pos);
    PawnEntry* pe = probe_pawn_table(pos);
    assert(me != nullptr && pe != nullptr);

    // 2. Tính điểm cơ bản
    SF::Score score = CalculatePSQT(pos);
    score += me->imbalance();
    score += pe->scores[SF::WHITE] - pe->scores[SF::BLACK];

    // 3. Khởi tạo EvalInfo
    EvalInfo::Info ei(pos, pe, me);

    // 4. Tính điểm các thành phần chi tiết
    SF::Score mobilityAndPiecesScore = SF::SCORE_ZERO; // Gộp điểm quân và mobility
    SF::Score kingSafetyScore = SF::SCORE_ZERO;
    SF::Score threatScore = SF::SCORE_ZERO;
    SF::Score passedPawnScore = SF::SCORE_ZERO;
    SF::Score spaceScore = SF::SCORE_ZERO;

    // --- 4.1 Mobility AND Piece Placement/Bonuses ---
    for (SF::Color c : {SF::WHITE, SF::BLACK}) {
        SF::Score side_score = SF::SCORE_ZERO; // Điểm cho quân và mobility của bên c
        SF::Direction downDir = (c == SF::WHITE) ? SF::SOUTH : SF::NORTH; // Hướng lùi
        SF::Bitboard outpostRanks = (c == SF::WHITE) ? (SF::Rank4BB | SF::Rank5BB | SF::Rank6BB)
                                                    : (SF::Rank5BB | SF::Rank4BB | SF::Rank3BB);
        SF::Square kingSq = static_cast<SF::Square>(Sq120ToSq64[pos->KingSq[c]]); // Vị trí vua bên c

        // Lặp qua các loại quân (N, B, R, Q)
        for (SF::PieceType pt : {SF::KNIGHT, SF::BISHOP, SF::ROOK, SF::QUEEN}) {
             SF::Bitboard pieces = ei.pieces(c, pt);
             int vicePieceEnumBase = (c == SF::WHITE) ? wN : bN; // Enum Vice tương ứng
             if(pt == SF::BISHOP) vicePieceEnumBase = (c == SF::WHITE) ? wB : bB;
             else if(pt == SF::ROOK) vicePieceEnumBase = (c == SF::WHITE) ? wR : bR;
             else if(pt == SF::QUEEN) vicePieceEnumBase = (c == SF::WHITE) ? wQ : bQ;

             // Lấy bitboard tấn công của Tốt (cả 2 bên) từ PawnEntry
             SF::Bitboard whitePawnAttacks = pe->pawnAttacks[SF::WHITE];
             SF::Bitboard blackPawnAttacks = pe->pawnAttacks[SF::BLACK];
             SF::Bitboard ourPawnAttacks = (c == SF::WHITE) ? whitePawnAttacks : blackPawnAttacks;
             SF::Bitboard theirPawnAttacks = (c == SF::WHITE) ? blackPawnAttacks : whitePawnAttacks;
             SF::Bitboard theirPawnAttackSpan = pe->pawnAttacksSpan[~c]; // Lấy từ PawnEntry

             while (pieces) {
                 SF::Square s = SF::pop_lsb(&pieces);
                 SF::Bitboard attacks_from_s;
                 SF::Bitboard occupied = ei.pieces();

                 // Tính tấn công & Mobility (Giống phiên bản trước)
                 if (pt == SF::KNIGHT)      attacks_from_s = AttackGen::PseudoAttacks_KNIGHT[s];
                 else if (pt == SF::BISHOP) attacks_from_s = AttackGen::attacks_from_sliding(SF::square_bb(s), occupied, AttackGen::BishopDirections, 4);
                 else if (pt == SF::ROOK)   attacks_from_s = AttackGen::attacks_from_sliding(SF::square_bb(s), occupied, AttackGen::RookDirections, 4);
                 else if (pt == SF::QUEEN)  attacks_from_s = AttackGen::attacks_from_sliding(SF::square_bb(s), occupied, AttackGen::BishopDirections, 4)
                                                           | AttackGen::attacks_from_sliding(SF::square_bb(s), occupied, AttackGen::RookDirections, 4);
                 else continue;

                 int mob_count = SF::popcount(attacks_from_s & ei.mobilityArea[c]);
                 int bonus_index = pt - SF::KNIGHT;
                 int max_mob = (pt == SF::KNIGHT) ? 8 : (pt == SF::BISHOP) ? 13 : (pt == SF::ROOK) ? 14 : 27;
                 mob_count = std::min(mob_count, max_mob);
                 side_score += EvaluationConstants::MobilityBonus[bonus_index][mob_count];

                 // *** Bổ sung điểm vị trí và bonus đặc biệt cho từng quân ***

                 if (pt == SF::KNIGHT || pt == SF::BISHOP) {
                     // Outpost: Ô trên rank 4/5/6, được Tốt ta bảo vệ, không bị Tốt địch kiểm soát span
                     SF::Bitboard outpostSquares = outpostRanks & ourPawnAttacks & ~theirPawnAttackSpan;
                     if (outpostSquares & s) { // Quân đang đứng trên outpost
                          side_score += EvaluationConstants::Outpost * (pt == SF::KNIGHT ? 2 : 1);
                     } else if (pt == SF::KNIGHT && (attacks_from_s & outpostSquares & ~ei.pieces(c))) { // Mã có thể nhảy tới outpost trống
                          side_score += EvaluationConstants::ReachableOutpost;
                     }

                     // Minor behind pawn: Quân nhẹ ngay sau Tốt đồng minh
                     if (SF::shift<downDir>(ei.pieces(c, SF::PAWN)) & s) {
                          side_score += EvaluationConstants::MinorBehindPawn;
                     }

                     // King Protector: Phạt nếu quân ở xa vua
                     if(SF::is_ok(kingSq)) {
                         side_score -= EvaluationConstants::KingProtector * SF::distance(s, kingSq);
                     }

                     if (pt == SF::BISHOP) {
                         // Bishop Pawns: Phạt dựa trên số Tốt cùng màu ô Tượng
                         SF::Bitboard pawnsOnSameColor = SF::SCORE_ZERO; // Cần hàm kiểm tra màu ô
                         // TODO: Implement pos.pawns_on_same_color_squares(c, s) or similar logic
                         // side_score -= EvaluationConstants::BishopPawns * popcount(pawnsOnSameColor);

                         // Long Diagonal Bishop: Bonus nếu Tượng kiểm soát đường chéo dài qua trung tâm
                         SF::Bitboard centerSquares = SF::square_bb(SF::SQ_D4) | SF::square_bb(SF::SQ_E4) | SF::square_bb(SF::SQ_D5) | SF::square_bb(SF::SQ_E5);
                         if (SF::more_than_one(AttackGen::attacks_from_sliding(SF::square_bb(s), ei.pieces(SF::PAWN), AttackGen::BishopDirections, 4) & centerSquares)) {
                              side_score += EvaluationConstants::LongDiagonalBishop;
                         }
                         // TODO: CorneredBishop (chủ yếu cho Chess960, có thể bỏ qua)
                     }
                 } else if (pt == SF::ROOK) {
                      // Rook on Queen File: Bonus nếu cùng cột với Hậu (bất kỳ màu nào)
                      if (SF::file_bb(s) & (ei.pieces(SF::WHITE, SF::QUEEN) | ei.pieces(SF::BLACK, SF::QUEEN))) {
                          side_score += EvaluationConstants::RookOnQueenFile;
                      }

                      // Rook on File: Bonus nếu ở cột nửa mở hoặc mở hoàn toàn
                      SF::Bitboard fileBB = SF::file_bb(s);
                      bool semiOpenFileUs = !(ei.pieces(c, SF::PAWN) & fileBB);
                      if(semiOpenFileUs) {
                          bool openFile = !(ei.pieces(~c, SF::PAWN) & fileBB);
                          side_score += EvaluationConstants::RookOnFile[openFile ? 1 : 0];
                      }

                      // Trapped Rook: Phạt nếu Xe bị Vua chặn và ít di chuyển
                      // Logic SF11 kiểm tra mob <= 3 và vị trí tương đối với Vua
                      if (mob_count <= 3 && SF::is_ok(kingSq)) {
                           SF::File kf = SF::file_of(kingSq);
                           SF::File rf = SF::file_of(s);
                           if ((kf < SF::FILE_E) == (rf < kf)) { // Xe cùng cánh với Vua và bị chặn bởi Vua
                                // TODO: Kiểm tra quyền nhập thành (pos->castlePerm) nếu cần phạt nặng hơn
                                side_score -= EvaluationConstants::TrappedRook;
                           }
                      }
                 }
                 // TODO: Queen Penalties (WeakQueen) cần logic pin detection phức tạp

             } // end while(pieces)
        } // end for PieceType

         if (c == SF::WHITE) mobilityAndPiecesScore += side_score;
         else mobilityAndPiecesScore -= side_score;
    } // end for Color
    score += mobilityAndPiecesScore;


    // --- 4.2 King Safety (CẬP NHẬT) ---
    for (SF::Color c : {SF::WHITE, SF::BLACK}) {
        SF::Color them = ~c;
        SF::Score ks_score_for_side = SF::SCORE_ZERO; // Bắt đầu tính điểm safety cho bên c
        int kingSq120 = pos->KingSq[c];
        int kingSq64 = Sq120ToSq64[kingSq120];

        if (kingSq64 < 0 || kingSq64 >= 64) continue;
        SF::Square ksq = static_cast<SF::Square>(kingSq64);

        // *** GỌI HÀM TÍNH SHELTER/STORM TỪ EXTENSIONS ***
        ks_score_for_side += PawnAnalysisExtensions::CalculateKingShelterAndStorm(pos, c);
        // Lưu ý: Hàm này trả về điểm (bonus cho shelter, penalty cho storm).
        // Nếu bạn muốn lưu nó vào PawnEntry, bạn cần sửa đổi pawn_analysis.cpp
        // và gọi hàm này từ đó, sau đó lấy giá trị từ pe->kingSafety[c] ở đây.

        // Tính điểm nguy hiểm từ tấn công và safe checks (Giống phiên bản trước)
        int kingDanger = 0;
        kingDanger += ei.kingAttackersCount[them] * 10; // Ví dụ
        kingDanger += ei.kingAttackersWeight[them];

        SF::Bitboard safeChecksArea = ~ei.pieces(c);
        safeChecksArea &= (~ei.attackedBy[c][SF::ALL_PIECES] | (ei.attackedBy[them][SF::ALL_PIECES] & ei.attackedBy2[c]));
        SF::Bitboard occupied_minus_our_queen = ei.pieces() ^ ei.pieces(c, SF::QUEEN);
        SF::Bitboard potential_rook_checks   = AttackGen::attacks_from_sliding(SF::square_bb(ksq), occupied_minus_our_queen, AttackGen::RookDirections, 4);
        SF::Bitboard potential_bishop_checks = AttackGen::attacks_from_sliding(SF::square_bb(ksq), occupied_minus_our_queen, AttackGen::BishopDirections, 4);
        SF::Bitboard potential_knight_checks = AttackGen::PseudoAttacks_KNIGHT[ksq];

        if (SF::popcount(ei.attackedBy[them][SF::ROOK]   & potential_rook_checks   & safeChecksArea)) kingDanger += EvaluationConstants::RookSafeCheck;
        if (SF::popcount(ei.attackedBy[them][SF::QUEEN]  & (potential_rook_checks | potential_bishop_checks) & safeChecksArea)) kingDanger += EvaluationConstants::QueenSafeCheck;
        if (SF::popcount(ei.attackedBy[them][SF::BISHOP] & potential_bishop_checks & safeChecksArea)) kingDanger += EvaluationConstants::BishopSafeCheck;
        if (SF::popcount(ei.attackedBy[them][SF::KNIGHT] & potential_knight_checks & safeChecksArea)) kingDanger += EvaluationConstants::KnightSafeCheck;

        File kf = SF::file_of(ksq);
        SF::Bitboard kingFlankMask = (kf <= SF::FILE_C) ? (SF::FileABB | SF::FileBBB | SF::FileCBB) :
                                     (kf >= SF::FILE_F) ? (SF::FileFBB | SF::FileGBB | SF::FileHBB) :
                                                          (SF::FileDBB | SF::FileEBB | SF::FileFBB);
        SF::Bitboard enemyAttacksInFlank = ei.attackedBy[them][SF::ALL_PIECES] & kingFlankMask;
        int kingFlankAttackCount = SF::popcount(enemyAttacksInFlank);
        kingDanger += kingFlankAttackCount * SF::mg_value(EvaluationConstants::FlankAttacks);

        // Chuyển kingDanger thành điểm phạt
        if (kingDanger > 100) {
             ks_score_for_side -= SF::make_score(kingDanger * kingDanger / 4096, kingDanger / 16);
        }

        // Phạt Vua ở cánh không Tốt
        if (!(ei.pieces(c, SF::PAWN) & kingFlankMask)) {
            ks_score_for_side -= EvaluationConstants::PawnlessFlank;
        }

        // Cộng/trừ điểm safety vào tổng điểm
        if (c == SF::WHITE) kingSafetyScore += ks_score_for_side;
        else kingSafetyScore -= ks_score_for_side;
    }
    score += kingSafetyScore;

    // --- 4.3 Threats (Giữ nguyên logic cơ bản như phiên bản trước) ---
    // TODO: Bổ sung logic ThreatBySafePawn, ThreatByPawnPush nếu cần
    for (SF::Color c : {SF::WHITE, SF::BLACK}) {
        SF::Color them = ~c;
        SF::Score threat_bonus = SF::SCORE_ZERO;
        SF::Bitboard nonPawnEnemies = ei.pieces(them) & ~ei.pieces(them, SF::PAWN);
        SF::Bitboard stronglyProtected = ei.attackedBy[them][SF::PAWN] | (ei.attackedBy2[them] & ~ei.attackedBy2[c]);
        SF::Bitboard weakOrDefendedEnemies = ei.pieces(them) & ei.attackedBy[c][SF::ALL_PIECES];
        SF::Bitboard weakEnemies = weakOrDefendedEnemies & ~stronglyProtected;

        SF::Bitboard minorAttacks = ei.attackedBy[c][SF::KNIGHT] | ei.attackedBy[c][SF::BISHOP];
        SF::Bitboard targets = weakOrDefendedEnemies & minorAttacks;
        while(targets) { /* ... ThreatByMinor ... */ }

        SF::Bitboard rookAttacks = ei.attackedBy[c][SF::ROOK];
        targets = weakEnemies & rookAttacks;
         while(targets) { /* ... ThreatByRook ... */ }

        if (weakEnemies & ei.attackedBy[c][SF::KING]) threat_bonus += EvaluationConstants::ThreatByKing;

        SF::Bitboard safeForEnemy = ~ei.attackedBy[them][SF::ALL_PIECES] | (nonPawnEnemies & ei.attackedBy2[c]);
        threat_bonus += EvaluationConstants::Hanging * SF::popcount(weakEnemies & safeForEnemy);

         SF::Bitboard restrictedTargets = ei.attackedBy[them][SF::ALL_PIECES] & ~stronglyProtected & ei.attackedBy[c][SF::ALL_PIECES];
         threat_bonus += EvaluationConstants::RestrictedPiece * SF::popcount(restrictedTargets);

        if (c == SF::WHITE) threatScore += threat_bonus;
        else threatScore -= threat_bonus;
    }
    score += threatScore;


    // --- 4.4 Passed Pawns (Giữ nguyên logic như phiên bản trước) ---
    for (SF::Color c : {SF::WHITE, SF::BLACK}) { /* ... */ }
    score += passedPawnScore;

    // --- 4.5 Space (Giữ nguyên logic như phiên bản trước) ---
     SF::Value npm_w = non_pawn_material(pos, SF::WHITE);
     SF::Value npm_b = non_pawn_material(pos, SF::BLACK);
     if (npm_w + npm_b >= EvaluationConstants::SpaceThreshold) { /* ... */ }
     score += spaceScore;

    // --- 4.6 Initiative (Giữ nguyên logic như phiên bản trước) ---
    SF::Value mg = SF::mg_value(score);
    SF::Value eg = SF::eg_value(score);
    int complexity = 0;
    complexity += SF::popcount(pe->passedPawns[SF::WHITE] | pe->passedPawns[SF::BLACK]) * 9;
    complexity += (pos->pceNum[wP] + pos->pceNum[bP]) * 3;
    complexity += pos->bigPce[SF::WHITE] + pos->bigPce[SF::BLACK];
    complexity -= 100;
    int initiative_mg = ((mg > 0) - (mg < 0)) * std::max(std::min(complexity + 50, 0), -std::abs(mg));
    int initiative_eg = ((eg > 0) - (eg < 0)) * std::max(complexity, -std::abs(eg));
    score += SF::make_score(initiative_mg, initiative_eg);


    // 5. Nội suy và Áp dụng Hệ số điều chỉnh (Giữ nguyên như phiên bản trước)
    SF::Phase gamePhase = me->gamePhase;
    SF::Value final_value;
    SF::ScaleFactor sf_w = SF::ScaleFactor(me->factor[SF::WHITE]);
    SF::ScaleFactor sf_b = SF::ScaleFactor(me->factor[SF::BLACK]);
    SF::ScaleFactor sf = (SF::eg_value(score) >= 0) ? sf_w : sf_b;

    final_value = (SF::mg_value(score) * int(gamePhase)
                 + SF::eg_value(score) * int(SF::PHASE_MIDGAME - gamePhase) * sf / SF::SCALE_FACTOR_NORMAL
                ) / SF::PHASE_MIDGAME;

    // 6. Thêm Tempo và trả về kết quả theo góc nhìn của bên đi
    final_value += EvaluationConstants::Tempo;

    return (pos->side == SF::WHITE) ? final_value : -final_value;
}