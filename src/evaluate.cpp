#include "evaluate.h"
#include "board.h"             
#include "types.h"             // Enum Piece gốc (wP, bK), Sq120ToSq64
#include "evaluation_types.h"  // SF::Score, SF::Value, SF::Color, etc.
#include "eval_info.h"         // EvalInfo::Info
#include "pawn_analysis.h"     // probe_pawn_table, PawnEntry
#include "material_analysis.h" // probe_material_table, MaterialEntry, non_pawn_material
#include "attack_gen.h"        // Các hàm tấn công, bảng PseudoAttacks, Directions
#include "psqt.h"
#include "init.h"
#include "bitboards.h" // debugging

#include <cassert>
#include <algorithm> 
#include <cmath>     
#include <vector>    
#include <sstream>

// --- Định nghĩa các hằng số (Lấy từ Stockfish 11 evaluate.cpp) ---
namespace EvaluationConstants {
    #define S(mg, eg) SF::make_score(mg, eg)

    const int KingAttackWeights[SF::PIECE_TYPE_NB] = { 0, 0, 81, 52, 44, 10 };

    const SF::Score MobilityBonus[][32] = {
      { S(-62,-81), S(-53,-56), S(-12,-30), S( -4,-14), S(  3,  8), S( 13, 15), // Knights
        S( 22, 23), S( 28, 27), S( 33, 33) },
      { S(-48,-59), S(-20,-23), S( 16, -3), S( 26, 13), S( 38, 24), S( 51, 42), // Bishops
        S( 55, 54), S( 63, 57), S( 63, 65), S( 68, 73), S( 81, 78), S( 81, 86),
        S( 91, 88), S( 98, 97) },
      { S(-58,-76), S(-27,-18), S(-15, 28), S(-10, 55), S( -5, 69), S( -2, 82), // Rooks
        S(  9,112), S( 16,118), S( 30,132), S( 29,142), S( 32,155), S( 38,165),
        S( 46,166), S( 48,169), S( 58,171) },
      { S(-39,-36), S(-21,-15), S(  3,  8), S(  3, 18), S( 14, 34), S( 22, 54), // Queens
        S( 28, 61), S( 41, 73), S( 43, 79), S( 48, 92), S( 56, 94), S( 60,104),
        S( 60,113), S( 66,120), S( 67,123), S( 70,126), S( 71,133), S( 73,136),
        S( 79,140), S( 88,143), S( 88,148), S( 99,166), S(102,170), S(102,175),
        S(106,184), S(109,191), S(113,206), S(116,212) }
    };

    const SF::Score ThreatByMinor[SF::PIECE_TYPE_NB] = {
      S(0, 0), S(6, 32), S(59, 41), S(79, 56), S(90, 119), S(79, 161), S(0,0) };
    const SF::Score ThreatByRook[SF::PIECE_TYPE_NB] = {
      S(0, 0), S(3, 44), S(38, 71), S(38, 61), S(0, 38), S(51, 38), S(0,0) };

    // const SF::Score WeakQueen          = S( 49, 15); // Bỏ qua vì cần logic pin phức tạp hơn
    // const SF::Score KnightOnQueen      = S( 16, 12); // Bỏ qua vì cần logic pin phức tạp hơn
    // const SF::Score SliderOnQueen      = S( 59, 18); // Bỏ qua vì cần logic pin phức tạp hơn

    const SF::Score PassedRank[SF::RANK_NB] = {
      S(0, 0), S(10, 28), S(17, 33), S(15, 41), S(62, 72), S(168, 177), S(276, 260), S(0, 0) };

    const int QueenSafeCheck  = 780;
    const int RookSafeCheck   = 1080;
    const int BishopSafeCheck = 635;
    const int KnightSafeCheck = 790;

    const SF::Value SpaceThreshold = SF::Value(12222);
    const SF::Value Tempo = SF::Value(28); // Lấy từ evaluation.h

    // Thêm các hằng số khác từ SF11 evaluate.cpp nếu cần cho các phần bạn sẽ bổ sung
    
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

// --- Hàm Helper Ánh xạ Enum Vice -> SF::PieceType (Lấy từ material_analysis.cpp) ---
// Đặt static để chỉ dùng trong file này
static inline SF::PieceType GetPieceTypeFromViceEnum(int vicePieceEnum) {
    switch (vicePieceEnum) {
        case wP: case bP: return SF::PAWN;
        case wN: case bN: return SF::KNIGHT;
        case wB: case bB: return SF::BISHOP;
        case wR: case bR: return SF::ROOK;
        case wQ: case bQ: return SF::QUEEN;
        case wK: case bK: return SF::KING;
        default:          return SF::NO_PIECE_TYPE;
    }
}

// --- Hàm tính điểm PSQT (Sử dụng pList và bảng PSQT::psq) ---
static SF::Score CalculatePSQT(const S_BOARD* pos) {
    SF::Score psqtScore = SF::SCORE_ZERO;
    for (int pc = wP; pc <= bK; ++pc) {
        if (pos->pieceNum[pc] > 0) {
            SF::Color color = (pc >= wP && pc <= wK) ? SF::WHITE : SF::BLACK;
            SF::PieceType pt = GetPieceTypeFromViceEnum(pc);
            if (pt == SF::NO_PIECE_TYPE) continue;

            SF::Piece sfPiece = SF::make_piece(color, pt);
            if (sfPiece == SF::NO_PIECE) continue;

            for (int i = 0; i < pos->pieceNum[pc]; ++i) {
                int sq120 = pos->pieceList[pc][i];
                if (sq120 != NO_SQ && sq120 != OFFBOARD) {
                    int sq64 = Sq120ToSq64[sq120];
                    if (sq64 >= 0 && sq64 < 64) {
                        // Truy cập bảng PSQT::psq từ psqt.cpp (đã được khởi tạo)
                        psqtScore += PSQT::psq[sfPiece][static_cast<SF::Square>(sq64)];
                    }
                }
            }
        }
    }
    return psqtScore;
}


// --- Hàm đánh giá chính ---
double EvaluatePosition(const S_BOARD* pos) {
    assert(pos != nullptr);
    // TODO: Thêm kiểm tra thế cờ có hợp lệ không (ví dụ: không có Vua đang bị chiếu)

    // 1. Lấy MaterialEntry và PawnEntry từ các hàm probe bạn đã viết
    MaterialEntry* me = probe_material_table(pos); //
    PawnEntry* pe = probe_pawn_table(pos);         //
    assert(me != nullptr && pe != nullptr);

    // 2. Tính điểm cơ bản
    SF::Score score = CalculatePSQT(pos);       // Điểm PSQT
    score += me->imbalance();                   // Điểm mất cân bằng vật chất
    score += pe->scores[SF::WHITE] - pe->scores[SF::BLACK]; // Điểm cấu trúc Tốt

    // 3. Khởi tạo EvalInfo (sử dụng constructor bạn đã viết)
    EvalInfo::Info ei(pos, pe, me); //

    // 4. Tính điểm các thành phần chi tiết
    SF::Score mobilityAndPiecesScore = SF::SCORE_ZERO;
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
        SF::Square kingSq = static_cast<SF::Square>(Sq120ToSq64[pos->kingSquare[c]]); // Vị trí vua bên c

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

                 ///-------------chia 2 để scale nhỏ lại do sợ nó bị tự tin quá mức------
                 side_score += EvaluationConstants::MobilityBonus[bonus_index][mob_count] /2;

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
                     if (SF::shift(ei.pieces(c, SF::PAWN), downDir) & s) {
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
                         SF::Bitboard all_pawns = (ei.pos_ptr->pawnsBB[SF::WHITE] | ei.pos_ptr->pawnsBB[SF::BLACK]);
                         if (SF::more_than_one(AttackGen::attacks_from_sliding(SF::square_bb(s), all_pawns, AttackGen::BishopDirections, 4) & centerSquares)) { // Sử dụng all_pawns
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

    // --- 4.2 King Safety ---
    for (SF::Color c : {SF::WHITE, SF::BLACK}) {
        SF::Color them = ~c;
        SF::Score ks_score_for_side = SF::SCORE_ZERO;
        int kingSq120 = pos->kingSquare[c]; //
        int kingSq64 = Sq120ToSq64[kingSq120]; //

        if (kingSq64 < 0 || kingSq64 >= 64) continue; // Vua không hợp lệ
        SF::Square ksq = static_cast<SF::Square>(kingSq64);

        // *** GỌI HÀM TÍNH SHELTER/STORM TỪ EXTENSIONS ***
        ks_score_for_side += CalculateKingShelterAndStorm(pos, c);

        // Lưu ý: Hàm này trả về điểm (bonus cho shelter, penalty cho storm).
        // Nếu bạn muốn lưu nó vào PawnEntry, bạn cần sửa đổi pawn_analysis.cpp
        // và gọi hàm này từ đó, sau đó lấy giá trị từ pe->kingSafety[c] ở đây.


        int kingDanger = 0;
        // Đếm số quân địch tấn công vùng vua (kingRing) và trọng số của chúng (lấy từ ei)
        kingDanger += ei.kingAttackersCount[them] * 10; // Ví dụ đơn giản hóa
        kingDanger += ei.kingAttackersWeight[them];     // Trọng số đã tính trong ei

        // Phân tích các check an toàn (Safe Checks)
        SF::Bitboard safeChecksArea = ~ei.pieces(c);
        safeChecksArea &= (~ei.attackedBy[c][SF::ALL_PIECES] | (ei.attackedBy[them][SF::ALL_PIECES] & ei.attackedBy2[c]));

        // Tính các ô có thể chiếu vua từ vị trí hiện tại
        SF::Bitboard occupied_minus_our_queen = ei.pieces() ^ ei.pieces(c, SF::QUEEN);
        SF::Bitboard potential_rook_checks   = AttackGen::attacks_from_sliding(SF::square_bb(ksq), occupied_minus_our_queen, AttackGen::RookDirections, 4);   //
        SF::Bitboard potential_bishop_checks = AttackGen::attacks_from_sliding(SF::square_bb(ksq), occupied_minus_our_queen, AttackGen::BishopDirections, 4); //
        SF::Bitboard potential_knight_checks = AttackGen::PseudoAttacks_KNIGHT[ksq]; //

        // Cộng điểm nguy hiểm nếu quân địch có thể chiếu an toàn
        if (SF::popcount(ei.attackedBy[them][SF::ROOK]   & potential_rook_checks   & safeChecksArea)) kingDanger += EvaluationConstants::RookSafeCheck;
        if (SF::popcount(ei.attackedBy[them][SF::QUEEN]  & (potential_rook_checks | potential_bishop_checks) & safeChecksArea)) kingDanger += EvaluationConstants::QueenSafeCheck;
        if (SF::popcount(ei.attackedBy[them][SF::BISHOP] & potential_bishop_checks & safeChecksArea)) kingDanger += EvaluationConstants::BishopSafeCheck;
        if (SF::popcount(ei.attackedBy[them][SF::KNIGHT] & potential_knight_checks & safeChecksArea)) kingDanger += EvaluationConstants::KnightSafeCheck;

        // Tấn công vào cánh Vua
        SF::File kf = SF::file_of(ksq);
        SF::Bitboard kingFlankMask = (kf <= SF::FILE_C) ? (SF::FileABB | SF::FileBBB | SF::FileCBB) :
                                     (kf >= SF::FILE_F) ? (SF::FileFBB | SF::FileGBB | SF::FileHBB) :
                                                          (SF::FileDBB | SF::FileEBB | SF::FileFBB);
        SF::Bitboard enemyAttacksInFlank = ei.attackedBy[them][SF::ALL_PIECES] & kingFlankMask;
        // SF11 có logic phức tạp hơn để xác định "Camp" và tính điểm dựa trên số lượng tấn công/phòng thủ
        int kingFlankAttackCount = SF::popcount(enemyAttacksInFlank); // Đơn giản hóa
        kingDanger += kingFlankAttackCount * SF::mg_value(EvaluationConstants::FlankAttacks);

        // Chuyển đổi kingDanger thành điểm phạt
        if (kingDanger > 100) {
            ks_score_for_side -= SF::make_score(kingDanger * kingDanger / 4096, kingDanger / 16);
        }

        // Phạt nếu Vua ở cánh không có Tốt
        if (!(ei.pieces(c, SF::PAWN) & kingFlankMask)) {
            ks_score_for_side -= EvaluationConstants::PawnlessFlank;
        }

        if (c == SF::WHITE) kingSafetyScore += ks_score_for_side;
        else kingSafetyScore -= ks_score_for_side;
    }
    score += kingSafetyScore;


    // --- 4.3 Threats ---
    for (SF::Color c : {SF::WHITE, SF::BLACK}) {
        SF::Color them = ~c;
        SF::Score threat_bonus = SF::SCORE_ZERO;

        SF::Bitboard nonPawnEnemies = ei.pieces(them) & ~ei.pieces(them, SF::PAWN);
        SF::Bitboard stronglyProtected = ei.attackedBy[them][SF::PAWN] | (ei.attackedBy2[them] & ~ei.attackedBy2[c]);
        SF::Bitboard weakOrDefendedEnemies = ei.pieces(them) & ei.attackedBy[c][SF::ALL_PIECES]; // Quân địch bị ta tấn công
        SF::Bitboard weakEnemies = weakOrDefendedEnemies & ~stronglyProtected; // Quân địch yếu (bị tấn công và không được bảo vệ mạnh)

        // Tấn công bằng quân Nhẹ (N, B)
        SF::Bitboard minorAttacks = ei.attackedBy[c][SF::KNIGHT] | ei.attackedBy[c][SF::BISHOP];
        SF::Bitboard targets = weakOrDefendedEnemies & minorAttacks;
        while(targets) {
             SF::Square targetSq = SF::pop_lsb(&targets);
             int targetPieceVice = pos->pieces[SQ120(targetSq)];
             SF::PieceType targetType = GetPieceTypeFromViceEnum(targetPieceVice);
             if(targetType != SF::NO_PIECE_TYPE && targetType <= SF::QUEEN) {
                threat_bonus += EvaluationConstants::ThreatByMinor[targetType];
             }
        }

        // Tấn công quân yếu bằng Xe
        SF::Bitboard rookAttacks = ei.attackedBy[c][SF::ROOK];
        targets = weakEnemies & rookAttacks; // Chỉ xét quân yếu khi bị Xe tấn công
         while(targets) {
             SF::Square targetSq = SF::pop_lsb(&targets);
             int targetPieceVice = pos->pieces[SQ120(targetSq)];
             SF::PieceType targetType = GetPieceTypeFromViceEnum(targetPieceVice);
              if(targetType != SF::NO_PIECE_TYPE && targetType <= SF::QUEEN) {
                 threat_bonus += EvaluationConstants::ThreatByRook[targetType];
              }
         }

        // Tấn công quân yếu bằng Vua
        if (weakEnemies & ei.attackedBy[c][SF::KING]) {
            threat_bonus += EvaluationConstants::ThreatByKing;
        }

        // Treo quân (Hanging) - Quân yếu không được địch bảo vệ hoặc bị ta tấn công >= 2 lần
        SF::Bitboard safeForEnemy = ~ei.attackedBy[them][SF::ALL_PIECES] | (nonPawnEnemies & ei.attackedBy2[c]);
        threat_bonus += EvaluationConstants::Hanging * SF::popcount(weakEnemies & safeForEnemy);

        // Hạn chế quân địch (RestrictedPiece) - Quân địch bị cả 2 bên tấn công và không được bảo vệ mạnh
         SF::Bitboard restrictedTargets = ei.attackedBy[them][SF::ALL_PIECES]
                                         & ~stronglyProtected
                                         & ei.attackedBy[c][SF::ALL_PIECES];
         threat_bonus += EvaluationConstants::RestrictedPiece * SF::popcount(restrictedTargets);

        // TODO: Bổ sung ThreatBySafePawn, ThreatByPawnPush (cần logic phức tạp hơn để xác định Tốt "an toàn")

        if (c == SF::WHITE) threatScore += threat_bonus;
        else threatScore -= threat_bonus;
    }
    score += threatScore;

    // --- 4.4 Passed Pawns ---
    for (SF::Color c : {SF::WHITE, SF::BLACK}) {
        SF::Color them = ~c;
        SF::Score passed_bonus_total = SF::SCORE_ZERO;
        SF::Bitboard passers = pe->passedPawns[c]; // Lấy từ PawnEntry

        while(passers) {
            SF::Square s = SF::pop_lsb(&passers);
            SF::Rank r_rel = SF::relative_rank(c, s);

            SF::Score bonus = EvaluationConstants::PassedRank[r_rel];

            // Bonus/penalty dựa vào khoảng cách Vua (chủ yếu EG)
            if (r_rel >= SF::RANK_4) {
                SF::Square blockSq = static_cast<SF::Square>(s + (c == SF::WHITE ? SF::NORTH : SF::SOUTH));
                if(SF::is_ok(blockSq)) {
                    SF::Square ourKingSq   = static_cast<SF::Square>(Sq120ToSq64[pos->kingSquare[c]]);
                    SF::Square theirKingSq = static_cast<SF::Square>(Sq120ToSq64[pos->kingSquare[them]]);
                    // Lấy khoảng cách Chebyshev (distance) đã tính trong SF
                    int dist_us = SF::is_ok(ourKingSq) ? SF::distance<SF::Square>(ourKingSq, blockSq) : 8;
                    int dist_them = SF::is_ok(theirKingSq) ? SF::distance<SF::Square>(theirKingSq, blockSq) : 8;

                    bonus += SF::make_score(0, ( (dist_them * 19) / 4 - dist_us * 2) * (5 * r_rel - 7) );

                    // Bonus thêm nếu Tốt được tự do tiến (ô chặn trống và an toàn)
                    SF::Bitboard blockSqBB = SF::square_bb(blockSq);
                    if (pos->pieces[SQ120(blockSq)] == EMPTY) {
                        SF::Bitboard path = SF::forward_file_bb(c, s);
                        SF::Bitboard unsafePath = path & ei.attackedBy[them][SF::ALL_PIECES];
                        int safety_k = !unsafePath                ? 35 : // Đường đi hoàn toàn an toàn
                                       !(unsafePath & blockSqBB) ? 20 : // Ô chặn an toàn
                                                                   9 ;  // Ô chặn bị tấn công
                         bonus += SF::make_score(safety_k * (5*r_rel -7), safety_k * (5*r_rel -7));
                    }
                }
            }
            // Phạt theo file (xa trung tâm hơn)
            bonus -= EvaluationConstants::PassedFile * SF::map_to_queenside(SF::file_of(s));

            passed_bonus_total += bonus;
        }
         if (c == SF::WHITE) passedPawnScore += passed_bonus_total;
         else passedPawnScore -= passed_bonus_total;
    }
    score += passedPawnScore;

    // --- 4.5 Space ---
    // Sử dụng hàm non_pawn_material từ material_analysis.cpp
     SF::Value npm_w = non_pawn_material(pos, SF::WHITE);
     SF::Value npm_b = non_pawn_material(pos, SF::BLACK);
     if (npm_w + npm_b >= EvaluationConstants::SpaceThreshold) {
         for (SF::Color c : {SF::WHITE, SF::BLACK}) {
             SF::Color them = ~c;
             SF::Direction down = (c == SF::WHITE) ? SF::SOUTH : SF::NORTH;
             // Mask cho các ô không gian quan trọng (rank 2-4 cho Trắng, 5-7 cho Đen, cột c-f)
             SF::Bitboard spaceMask = (SF::FileCBB | SF::FileDBB | SF::FileEBB | SF::FileFBB) &
                 ((c == SF::WHITE) ? (SF::Rank2BB | SF::Rank3BB | SF::Rank4BB)
                                   : (SF::Rank7BB | SF::Rank6BB | SF::Rank5BB));

             // Ô an toàn = trong mask, không có Tốt mình, không bị Tốt địch tấn công
             SF::Bitboard safeSpace = spaceMask & ~ei.pieces(c, SF::PAWN) & ~ei.attackedBy[them][SF::PAWN];

             // Ô phía sau Tốt mình (tối đa 3 hàng)
             SF::Bitboard behindPawns = ei.pieces(c, SF::PAWN);
             behindPawns |= SF::shift(behindPawns, down); // Dùng hàm shift(bitboard, direction)
             SF::Direction two_down = static_cast<SF::Direction>(down + down);
             behindPawns |= SF::shift(behindPawns, two_down); // Dùng hàm shift(bitboard, direction)

             // Đếm ô không gian an toàn + ô an toàn phía sau Tốt (không bị địch tấn công)
             int space_bonus_count = SF::popcount(safeSpace)
                                   + SF::popcount(behindPawns & safeSpace & ~ei.attackedBy[them][SF::ALL_PIECES]);

             // Đếm số quân khác Tốt của mình
             int pieceCount = pos->bigPiece[c] - pos->pieceNum[(c == SF::WHITE) ? wK : bK]; // Trừ Vua ra

             SF::Score space_bonus = SF::make_score(space_bonus_count * pieceCount * pieceCount / 16, 0); // Chỉ tính MG

             if (c == SF::WHITE) spaceScore += space_bonus;
             else spaceScore -= space_bonus;
         }
         score += spaceScore;
     }

    // --- 4.6 Initiative (Tính cuối cùng) ---
    SF::Value mg = SF::mg_value(score);
    SF::Value eg = SF::eg_value(score);
    // Tính độ phức tạp (simplified version of SF11 logic)
    int complexity = 0;
    complexity += SF::popcount(pe->passedPawns[SF::WHITE] | pe->passedPawns[SF::BLACK]) * 9; // Số Tốt thông
    complexity += (pos->pieceNum[wP] + pos->pieceNum[bP]) * 3; // Số Tốt tổng
    complexity += pos->bigPiece[SF::WHITE] + pos->bigPiece[SF::BLACK]; // Số quân lớn
    // Thêm các yếu tố khác nếu muốn...
    complexity -= 100; // Giảm cơ sở

    // Áp dụng bonus/malus dựa trên bên đang thắng và độ phức tạp
    int initiative_mg = ((mg > 0) - (mg < 0)) * std::max(std::min(complexity + 50, 0), -std::abs(mg));
    int initiative_eg = ((eg > 0) - (eg < 0)) * std::max(complexity, -std::abs(eg));
    score += SF::make_score(initiative_mg, initiative_eg);

    // 5. Nội suy và Áp dụng Hệ số điều chỉnh
    SF::Phase gamePhase = me->gamePhase; //
    SF::Value final_value;

    // Lấy hệ số điều chỉnh mặc định đã tính trong material_analysis.cpp
    SF::ScaleFactor sf_w = SF::ScaleFactor(me->factor[SF::WHITE]); //
    SF::ScaleFactor sf_b = SF::ScaleFactor(me->factor[SF::BLACK]); //

    // Chọn hệ số của bên mạnh hơn (dựa trên điểm EG)
    SF::ScaleFactor sf = (SF::eg_value(score) >= 0) ? sf_w : sf_b;

    // --- PHẦN NÀY BỎ QUA vì MaterialEntry chưa hỗ trợ scaling function đặc biệt ---
    // // Nếu có hàm scaling đặc biệt, dùng nó thay thế
    // const auto* scaleFunc = me->scalingFunction[(SF::eg_value(score) >= 0 ? SF::WHITE : SF::BLACK)];
    // if (scaleFunc) {
    //     // sf = (*scaleFunc)(pos); // Cần hàm nhận S_BOARD*
    // }
    // --- KẾT THÚC PHẦN BỎ QUA ---

    // Nội suy giữa MG và EG, áp dụng hệ số scale cho phần EG
    final_value = (SF::mg_value(score) * int(gamePhase)
                 + SF::eg_value(score) * int(SF::PHASE_MIDGAME - gamePhase) * sf / SF::SCALE_FACTOR_NORMAL
                ) / SF::PHASE_MIDGAME;

    // 6. Thêm Tempo và trả về kết quả theo góc nhìn của bên đi
    double result  = (pos->side == WHITE) ? final_value : -final_value;
    result += EvaluationConstants::Tempo; //

    return result; //don't convert to centipawns
}



// --- Đặt trong namespace EvalPhase3 của file evaluate.cpp ---

namespace EvalPhase3 {

    // --- Hàm phụ trợ định dạng điểm ---
    std::string format_score_trace(SF::Score s) {
        char buf[25];
        // Căn chỉnh tốt hơn một chút
        std::snprintf(buf, sizeof(buf), " %+6d %+6d ", SF::mg_value(s), SF::eg_value(s));
        return std::string(buf);
    }

    // --- Hàm tính PSQT riêng cho từng màu (Hàm helper mới) ---
    static void CalculatePSQT_ByColor(const S_BOARD* pos, SF::Score& psqt_w, SF::Score& psqt_b) {
        psqt_w = SF::SCORE_ZERO;
        psqt_b = SF::SCORE_ZERO;
        for (int pc = wP; pc <= bK; ++pc) {
            if (pos->pieceNum[pc] > 0) {
                SF::Color color = (pc >= wP && pc <= wK) ? SF::WHITE : SF::BLACK;
                SF::PieceType pt = GetPieceTypeFromViceEnum(pc); // Dùng lại hàm helper cũ
                if (pt == SF::NO_PIECE_TYPE) continue;

                SF::Piece sfPiece = SF::make_piece(color, pt);
                if (sfPiece == SF::NO_PIECE) continue;

                for (int i = 0; i < pos->pieceNum[pc]; ++i) {
                    int sq120 = pos->pieceList[pc][i];
                    if (sq120 != NO_SQ && sq120 != OFFBOARD) {
                        int sq64 = Sq120ToSq64[sq120];
                        if (sq64 >= 0 && sq64 < 64) {
                            SF::Score piece_score = PSQT::psq[sfPiece][static_cast<SF::Square>(sq64)];
                            if (color == SF::WHITE) {
                                psqt_w += piece_score;
                            } else {
                                // Điểm PSQT của Đen đã có dấu âm, nên trừ đi để thành dương
                                psqt_b -= piece_score;
                            }
                        }
                    }
                }
            }
        }
    }

    // --- Hàm Trace đã sửa lỗi ---
    std::string trace(const S_BOARD* pos) {
        assert(pos != nullptr);

        // 2. Lấy dữ liệu
        MaterialEntry* me = probe_material_table(pos);
        PawnEntry* pe = probe_pawn_table(pos);
        assert(me != nullptr && pe != nullptr);
        EvalInfo::Info ei(pos, pe, me);

        // --- Biến lưu trữ điểm từng thành phần ---
        SF::Score psqt_w_s = SF::SCORE_ZERO, psqt_b_s = SF::SCORE_ZERO;
        SF::Score imbalance_s = SF::SCORE_ZERO;
        SF::Score pawns_w_s = SF::SCORE_ZERO, pawns_b_s = SF::SCORE_ZERO;
        SF::Score mobility_pieces_w_s = SF::SCORE_ZERO, mobility_pieces_b_s = SF::SCORE_ZERO;
        SF::Score king_w_s = SF::SCORE_ZERO, king_b_s = SF::SCORE_ZERO;
        SF::Score threat_w_s = SF::SCORE_ZERO, threat_b_s = SF::SCORE_ZERO;
        SF::Score passed_w_s = SF::SCORE_ZERO, passed_b_s = SF::SCORE_ZERO;
        SF::Score space_w_s = SF::SCORE_ZERO, space_b_s = SF::SCORE_ZERO;
        SF::Score initiative_s = SF::SCORE_ZERO;
        SF::Score score_accumulator = SF::SCORE_ZERO; // Biến cộng dồn HIỆU SỐ (W - B)

        // 3. Tính điểm cơ bản
        CalculatePSQT_ByColor(pos, psqt_w_s, psqt_b_s);
        imbalance_s = me->imbalance();
        pawns_w_s = pe->scores[SF::WHITE];
        pawns_b_s = pe->scores[SF::BLACK]; // Giả định đây là điểm tuyệt đối của Đen (thường dương)

        // Tính hiệu số ban đầu cho score_accumulator
        score_accumulator = (psqt_w_s - psqt_b_s) + imbalance_s + (pawns_w_s - pawns_b_s);

        // 4. Tính điểm chi tiết

        // --- 4.1 Mobility & Pieces ---
        SF::Score current_mobility_w = SF::SCORE_ZERO;
        SF::Score current_mobility_b = SF::SCORE_ZERO;
        for (SF::Color c : {SF::WHITE, SF::BLACK}) {
            SF::Score side_score = SF::SCORE_ZERO;
            SF::Direction downDir = (c == SF::WHITE) ? SF::SOUTH : SF::NORTH;
            SF::Bitboard outpostRanks = (c == SF::WHITE) ? (SF::Rank4BB | SF::Rank5BB | SF::Rank6BB) : (SF::Rank5BB | SF::Rank4BB | SF::Rank3BB);
            SF::Square kingSq = SF::SQ_NONE;
            int kingSq120_side = pos->kingSquare[c];
             if(kingSq120_side != NO_SQ && kingSq120_side != OFFBOARD) {
                   int kingSq64_side = Sq120ToSq64[kingSq120_side];
                   if(kingSq64_side >= 0 && kingSq64_side < 64) kingSq = static_cast<SF::Square>(kingSq64_side);
             }
            SF::Bitboard whitePawnAttacks = pe->pawnAttacks[SF::WHITE];
            SF::Bitboard blackPawnAttacks = pe->pawnAttacks[SF::BLACK];
            SF::Bitboard ourPawnAttacks = (c == SF::WHITE) ? whitePawnAttacks : blackPawnAttacks;
            SF::Bitboard theirPawnAttacks = (c == SF::WHITE) ? blackPawnAttacks : whitePawnAttacks;
            SF::Bitboard theirPawnAttackSpan = pe->pawnAttacksSpan[~c];
            for (SF::PieceType pt : {SF::KNIGHT, SF::BISHOP, SF::ROOK, SF::QUEEN}) {
                 SF::Bitboard pieces = ei.pieces(c, pt);
                 while (pieces) {
                     SF::Square s = SF::pop_lsb(&pieces);
                     SF::Bitboard attacks_from_s = 0;
                     SF::Bitboard occupied = ei.pieces();
                     if (pt == SF::KNIGHT)      attacks_from_s = AttackGen::PseudoAttacks_KNIGHT[s];
                     else if (pt == SF::BISHOP) attacks_from_s = AttackGen::attacks_from_sliding(SF::square_bb(s), occupied, AttackGen::BishopDirections, 4);
                     else if (pt == SF::ROOK)   attacks_from_s = AttackGen::attacks_from_sliding(SF::square_bb(s), occupied, AttackGen::RookDirections, 4);
                     else if (pt == SF::QUEEN)  attacks_from_s = AttackGen::attacks_from_sliding(SF::square_bb(s), occupied, AttackGen::BishopDirections, 4) | AttackGen::attacks_from_sliding(SF::square_bb(s), occupied, AttackGen::RookDirections, 4);
                     else continue;
                     int mob_count = SF::popcount(attacks_from_s & ei.mobilityArea[c]);
                    

                     int bonus_index = pt - SF::KNIGHT;
                     int max_mob = (pt == SF::KNIGHT) ? 8 : (pt == SF::BISHOP) ? 13 : (pt == SF::ROOK) ? 14 : 27;
                     mob_count = std::min(mob_count, max_mob);
                     if (mob_count >= 0 && mob_count < 32 && bonus_index >= 0 && bonus_index < 4) {
                         side_score += EvaluationConstants::MobilityBonus[bonus_index][mob_count];
                            std::cout<<"mob_count: "<<mob_count<<std::endl;
                            

                         std::cout<<"side_score: "<<SF::mg_value(side_score)<<SF::eg_value(side_score)<<std::endl;
                     }
                     if (pt == SF::KNIGHT || pt == SF::BISHOP) {
                         SF::Bitboard outpostSquares = outpostRanks & ourPawnAttacks & ~theirPawnAttackSpan;
                        std::cout<<"outpostSquares: ";PrintBitBoard(outpostSquares);std::cout<<std::endl;

                         if (outpostSquares & s) { 
                            side_score += EvaluationConstants::Outpost * (pt == SF::KNIGHT ? 2 : 1); 
                            std::cout<<"side_score:1    "<<SF::mg_value(side_score)<<SF::eg_value(side_score)<<std::endl;
                        }
                         else if (pt == SF::KNIGHT && (attacks_from_s & outpostSquares & ~ei.pieces(c))) {
                             side_score += EvaluationConstants::ReachableOutpost; 
                             std::cout<<"side_score:2   "<<SF::mg_value(side_score)<<SF::eg_value(side_score)<<std::endl;}
                         if (SF::shift(ei.pieces(c, SF::PAWN), downDir) & s) { 
                            side_score += EvaluationConstants::MinorBehindPawn; 
                            std::cout<<"side_score:3   "<<SF::mg_value(side_score)<<SF::eg_value(side_score)<<std::endl;}
                         if(SF::is_ok(kingSq)) { 
                            side_score -= EvaluationConstants::KingProtector * SF::distance(s, kingSq); 
                        std::cout<<"side_score:4   "<<SF::mg_value(side_score)<<SF::eg_value(side_score)<<std::endl;}
                         if (pt == SF::BISHOP) {
                             SF::Bitboard centerSquares = SF::square_bb(SF::SQ_D4) | SF::square_bb(SF::SQ_E4) | SF::square_bb(SF::SQ_D5) | SF::square_bb(SF::SQ_E5);
                             std::cout<<"centerSquares: ";PrintBitBoard(centerSquares);std::cout<<std::endl;
                             SF::Bitboard all_pawns = (ei.pos_ptr->pawnsBB[SF::WHITE] | ei.pos_ptr->pawnsBB[SF::BLACK]);
                             std::cout<<"all_pawns: ";PrintBitBoard(all_pawns);std::cout<<std::endl;
                             if (SF::more_than_one(AttackGen::attacks_from_sliding(SF::square_bb(s), all_pawns, AttackGen::BishopDirections, 4) & centerSquares)) { 
                                side_score += EvaluationConstants::LongDiagonalBishop;
                                 std::cout<<"side_score:5   "<<SF::mg_value(side_score)<<SF::eg_value(side_score)<<std::endl;
                            }
                         }
                     } else if (pt == SF::ROOK) {
                          if (SF::file_bb(s) & (ei.pieces(SF::WHITE, SF::QUEEN) | ei.pieces(SF::BLACK, SF::QUEEN))) { 
                            side_score += EvaluationConstants::RookOnQueenFile; 
                            std::cout<<"side_score:6   "<<SF::mg_value(side_score)<<SF::eg_value(side_score)<<std::endl;}
                          SF::Bitboard fileBB = SF::file_bb(s);
                          bool semiOpenFileUs = !(ei.pieces(c, SF::PAWN) & fileBB);
                          if(semiOpenFileUs) { bool openFile = !(ei.pieces(~c, SF::PAWN) & fileBB); side_score += EvaluationConstants::RookOnFile[openFile ? 1 : 0]; }
                          if (mob_count <= 3 && SF::is_ok(kingSq)) { SF::File kf = SF::file_of(kingSq); SF::File rf = SF::file_of(s); if ((kf < SF::FILE_E) == (rf < kf)) { side_score -= EvaluationConstants::TrappedRook; } }
                     }
                 }
            }
            if (c == SF::WHITE) current_mobility_w = side_score;
            else current_mobility_b = side_score;
        }
        mobility_pieces_w_s = current_mobility_w;
        mobility_pieces_b_s = current_mobility_b;
        std::cout<<"mobility_pieces_w_s: "<<mobility_pieces_w_s<<std::endl;
        std::cout<<"mobility_pieces_b_s: "<<mobility_pieces_b_s<<std::endl;
        //score_accumulator += (mobility_pieces_w_s - mobility_pieces_b_s);

        // --- 4.2 King Safety ---
        SF::Score current_king_w = SF::SCORE_ZERO;
        SF::Score current_king_b = SF::SCORE_ZERO;
        for (SF::Color c : {SF::WHITE, SF::BLACK}) {
            SF::Score ks_score_for_side = SF::SCORE_ZERO;
            SF::Color them = ~c;
            int kingSq120 = pos->kingSquare[c];
            int kingSq64 = (kingSq120 != NO_SQ && kingSq120 != OFFBOARD) ? Sq120ToSq64[kingSq120] : -1;
            if (kingSq64 < 0 || kingSq64 >= 64) continue;
            SF::Square ksq = static_cast<SF::Square>(kingSq64);
            ks_score_for_side += CalculateKingShelterAndStorm(pos, c);

            int kingDanger = 0;
            kingDanger += ei.kingAttackersCount[them] * 10;
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
            SF::File kf = SF::file_of(ksq);
            SF::Bitboard kingFlankMask = (kf <= SF::FILE_C) ? (SF::FileABB | SF::FileBBB | SF::FileCBB) : ((kf >= SF::FILE_F) ? (SF::FileFBB | SF::FileGBB | SF::FileHBB) : (SF::FileDBB | SF::FileEBB | SF::FileFBB));
            SF::Bitboard enemyAttacksInFlank = ei.attackedBy[them][SF::ALL_PIECES] & kingFlankMask;
            int kingFlankAttackCount = SF::popcount(enemyAttacksInFlank);
            kingDanger += kingFlankAttackCount * SF::mg_value(EvaluationConstants::FlankAttacks);
            if (kingDanger > 100) { ks_score_for_side -= SF::make_score(kingDanger * kingDanger / 4096, kingDanger / 16); }
            if (!(ei.pieces(c, SF::PAWN) & kingFlankMask)) { ks_score_for_side -= EvaluationConstants::PawnlessFlank; }


            if (c == SF::WHITE) current_king_w = ks_score_for_side;
            else current_king_b = ks_score_for_side;
        }
        king_w_s = current_king_w;
        king_b_s = current_king_b;
        score_accumulator += (king_w_s - king_b_s);

        // --- 4.3 Threats ---
        SF::Score current_threat_w = SF::SCORE_ZERO;
        SF::Score current_threat_b = SF::SCORE_ZERO;
        for (SF::Color c : {SF::WHITE, SF::BLACK}) {
            SF::Score threat_bonus = SF::SCORE_ZERO;
            SF::Color them = ~c;
            SF::Bitboard nonPawnEnemies = ei.pieces(them) & ~ei.pieces(them, SF::PAWN);
            SF::Bitboard stronglyProtected = ei.attackedBy[them][SF::PAWN] | (ei.attackedBy2[them] & ~ei.attackedBy2[c]);
            SF::Bitboard weakOrDefendedEnemies = ei.pieces(them) & ei.attackedBy[c][SF::ALL_PIECES];
            SF::Bitboard weakEnemies = weakOrDefendedEnemies & ~stronglyProtected;
            SF::Bitboard minorAttacks = ei.attackedBy[c][SF::KNIGHT] | ei.attackedBy[c][SF::BISHOP];
            SF::Bitboard targets_minor = weakOrDefendedEnemies & minorAttacks;

            
            while(targets_minor) {
                 SF::Square targetSq = SF::pop_lsb(&targets_minor);
                 int sq120 = SQ120(targetSq);
                 if(SF::is_ok(targetSq) && sq120 != NO_SQ && sq120 != OFFBOARD){
                    int targetPieceVice = pos->pieces[sq120];
                    SF::PieceType targetType = GetPieceTypeFromViceEnum(targetPieceVice);
                    if(targetType >= SF::PAWN && targetType <= SF::KING) { threat_bonus += EvaluationConstants::ThreatByMinor[targetType]; }
                 }
            }
            SF::Bitboard rookAttacks = ei.attackedBy[c][SF::ROOK];
            SF::Bitboard targets_rook = weakEnemies & rookAttacks;
             while(targets_rook) {
                 SF::Square targetSq = SF::pop_lsb(&targets_rook);
                 int sq120 = SQ120(targetSq);
                  if(SF::is_ok(targetSq) && sq120 != NO_SQ && sq120 != OFFBOARD){
                     int targetPieceVice = pos->pieces[sq120];
                     SF::PieceType targetType = GetPieceTypeFromViceEnum(targetPieceVice);
                     if(targetType >= SF::PAWN && targetType <= SF::KING) { threat_bonus += EvaluationConstants::ThreatByRook[targetType]; }
                  }
             }
            if (weakEnemies & ei.attackedBy[c][SF::KING]) { threat_bonus += EvaluationConstants::ThreatByKing; }
            SF::Bitboard safeForEnemy = ~ei.attackedBy[them][SF::ALL_PIECES] | (nonPawnEnemies & ei.attackedBy2[c]);
            threat_bonus += EvaluationConstants::Hanging * SF::popcount(weakEnemies & safeForEnemy);
            SF::Bitboard restrictedTargets = ei.attackedBy[them][SF::ALL_PIECES] & ~stronglyProtected & ei.attackedBy[c][SF::ALL_PIECES];
            threat_bonus += EvaluationConstants::RestrictedPiece * SF::popcount(restrictedTargets);

            if (c == SF::WHITE) current_threat_w = threat_bonus;
            else current_threat_b = threat_bonus;
        }
        threat_w_s = current_threat_w;
        threat_b_s = current_threat_b;
        score_accumulator += (threat_w_s - threat_b_s);

        // --- 4.4 Passed Pawns ---
        SF::Score current_passed_w = SF::SCORE_ZERO;
        SF::Score current_passed_b = SF::SCORE_ZERO;
        for (SF::Color c : {SF::WHITE, SF::BLACK}) {
            SF::Score passed_bonus_total = SF::SCORE_ZERO;
            SF::Color them = ~c;
            SF::Bitboard passers = pe->passedPawns[c];
            while(passers) {
                SF::Square s = SF::pop_lsb(&passers);
                SF::Rank r_rel = SF::relative_rank(c, s);
                if (r_rel >= SF::RANK_1 && r_rel < SF::RANK_NB) {
                    SF::Score bonus = EvaluationConstants::PassedRank[r_rel];
                    if (r_rel >= SF::RANK_4) {
                        SF::Square blockSq = static_cast<SF::Square>(s + (c == SF::WHITE ? SF::NORTH : SF::SOUTH));
                        if(SF::is_ok(blockSq)) {
                            SF::Square ourKingSq = SF::SQ_NONE, theirKingSq = SF::SQ_NONE;
                            int kingSq120_us = pos->kingSquare[c], kingSq120_them = pos->kingSquare[them];
                            if(kingSq120_us != NO_SQ && kingSq120_us != OFFBOARD) { int ksq64 = Sq120ToSq64[kingSq120_us]; if(ksq64>=0&&ksq64<64) ourKingSq=static_cast<SF::Square>(ksq64); }
                            if(kingSq120_them != NO_SQ && kingSq120_them != OFFBOARD) { int ksq64 = Sq120ToSq64[kingSq120_them]; if(ksq64>=0&&ksq64<64) theirKingSq=static_cast<SF::Square>(ksq64); }
                            int dist_us = SF::is_ok(ourKingSq) ? SF::distance(ourKingSq, blockSq) : 8;
                            int dist_them = SF::is_ok(theirKingSq) ? SF::distance(theirKingSq, blockSq) : 8;
                            bonus += SF::make_score(0, ( (dist_them * 19) / 4 - dist_us * 2) * (5 * r_rel - 7) );
                            int blockSq120 = SQ120(blockSq);
                            if (SF::is_ok(blockSq) && blockSq120 != NO_SQ && blockSq120 != OFFBOARD && pos->pieces[blockSq120] == EMPTY) {
                                 SF::Bitboard path = SF::forward_file_bb(c, s);
                                 SF::Bitboard unsafePath = path & ei.attackedBy[them][SF::ALL_PIECES];
                                 int safety_k = !unsafePath ? 35 : (!(unsafePath & SF::square_bb(blockSq)) ? 20 : 9);
                                 bonus += SF::make_score(safety_k * (5*r_rel -7), safety_k * (5*r_rel -7));
                            }
                        }
                    }
                    bonus -= EvaluationConstants::PassedFile * SF::map_to_queenside(SF::file_of(s));
                    passed_bonus_total += bonus;
                }
            }
            if (c == SF::WHITE) current_passed_w = passed_bonus_total;
            else current_passed_b = passed_bonus_total;
        }
        passed_w_s = current_passed_w;
        passed_b_s = current_passed_b;
        score_accumulator += (passed_w_s - passed_b_s);

        // --- 4.5 Space ---
        SF::Score current_space_w = SF::SCORE_ZERO;
        SF::Score current_space_b = SF::SCORE_ZERO;
        SF::Value npm_w = non_pawn_material(pos, SF::WHITE);

        SF::Value npm_b = non_pawn_material(pos, SF::BLACK);

        if (npm_w + npm_b >= EvaluationConstants::SpaceThreshold) {
            for (SF::Color c : {SF::WHITE, SF::BLACK}) {
                SF::Score space_bonus = SF::SCORE_ZERO;
                 SF::Color them = ~c;
                 SF::Direction down = (c == SF::WHITE) ? SF::SOUTH : SF::NORTH;
                 SF::Bitboard spaceMask = (SF::FileCBB | SF::FileDBB | SF::FileEBB | SF::FileFBB) & ((c == SF::WHITE) ? (SF::Rank2BB | SF::Rank3BB | SF::Rank4BB) : (SF::Rank7BB | SF::Rank6BB | SF::Rank5BB));
                 SF::Bitboard safeSpace = spaceMask & ~ei.pieces(c, SF::PAWN) & ~ei.attackedBy[them][SF::PAWN];
                 SF::Bitboard behindPawns = ei.pieces(c, SF::PAWN);
                 behindPawns |= SF::shift(behindPawns, down);
                 SF::Direction two_down = static_cast<SF::Direction>(down + down);
                 behindPawns |= SF::shift(behindPawns, two_down);
                 int space_bonus_count = SF::popcount(safeSpace) + SF::popcount(behindPawns & safeSpace & ~ei.attackedBy[them][SF::ALL_PIECES]);
    
                 int pieceCount = pos->bigPiece[c] - pos->pieceNum[(c == SF::WHITE) ? wK : bK];
                 space_bonus = SF::make_score(space_bonus_count * pieceCount * pieceCount / 16, 0);

                if (c == SF::WHITE) current_space_w = space_bonus;
                else current_space_b = space_bonus;
            }
        }
        space_w_s = current_space_w;
        space_b_s = current_space_b;
        score_accumulator += (space_w_s - space_b_s);

        // --- 4.6 Initiative ---
        SF::Value mg_before_init = SF::mg_value(score_accumulator);
        SF::Value eg_before_init = SF::eg_value(score_accumulator);
        int complexity = 0;
        complexity += SF::popcount(pe->passedPawns[SF::WHITE] | pe->passedPawns[SF::BLACK]) * 9;
        complexity += (pos->pieceNum[wP] + pos->pieceNum[bP]) * 3;
        complexity += pos->bigPiece[SF::WHITE] + pos->bigPiece[SF::BLACK];
        complexity -= 100;
        int initiative_mg_calc = ((mg_before_init > 0) - (mg_before_init < 0)) * std::max(std::min(complexity + 50, 0), -std::abs(mg_before_init));
        int initiative_eg_calc = ((eg_before_init > 0) - (eg_before_init < 0)) * std::max(complexity, -std::abs(eg_before_init));
        initiative_s = SF::make_score(initiative_mg_calc, initiative_eg_calc);
        score_accumulator += initiative_s; // Cộng initiative vào tổng hiệu số cuối cùng
        // 5. Tính giá trị nội suy cuối cùng
        SF::Value final_value_white_pov;
        SF::Phase gamePhase = me->gamePhase;
        SF::ScaleFactor sf_w = SF::ScaleFactor(me->factor[SF::WHITE]);
        SF::ScaleFactor sf_b = SF::ScaleFactor(me->factor[SF::BLACK]);
        // Chọn hệ số của bên mạnh hơn (dựa trên điểm EG)
        SF::ScaleFactor sf = (SF::eg_value(score_accumulator) >= 0) ? sf_w : sf_b;
        int phase_denominator = SF::PHASE_MIDGAME; if (phase_denominator == 0) phase_denominator = 1;
        final_value_white_pov = (SF::mg_value(score_accumulator) * int(gamePhase)
                               + SF::eg_value(score_accumulator) * int(SF::PHASE_MIDGAME - gamePhase) * sf / SF::SCALE_FACTOR_NORMAL
                              ) / phase_denominator;
        final_value_white_pov += EvaluationConstants::Tempo;

        // 6. Tạo chuỗi Output
        std::stringstream ss;
        // Header
        ss << " Term          |    White Score     |    Black Score     |      Total Score     \n"
           << "               |   MG      EG     |   MG      EG     |     MG      EG     \n"
           << "---------------+------------------+------------------+--------------------\n";

        // Điền bảng
        ss << " Material (PSQT)|" << format_score_trace(psqt_w_s) << "|" << format_score_trace(psqt_b_s) << "|" << format_score_trace(psqt_w_s - psqt_b_s) << "\n";
        ss << " Imbalance     |" << "                  |" << "                  |" << format_score_trace(imbalance_s) << "\n";
        ss << " Pawns         |" << format_score_trace(pawns_w_s) << "|" << format_score_trace(pawns_b_s) << "|" << format_score_trace(pawns_w_s - pawns_b_s) << "\n"; // Đã sửa Total
        ss << " Mobility/Piece|" << format_score_trace(mobility_pieces_w_s) << "|" << format_score_trace(mobility_pieces_b_s) << "|" << format_score_trace(mobility_pieces_w_s - mobility_pieces_b_s) << "\n";
        ss << " King safety   |" << format_score_trace(king_w_s) << "|" << format_score_trace(king_b_s) << "|" << format_score_trace(king_w_s - king_b_s) << "\n";
        ss << " Threats       |" << format_score_trace(threat_w_s) << "|" << format_score_trace(threat_b_s) << "|" << format_score_trace(threat_w_s - threat_b_s) << "\n";
        ss << " Passed pawns  |" << format_score_trace(passed_w_s) << "|" << format_score_trace(passed_b_s) << "|" << format_score_trace(passed_w_s - passed_b_s) << "\n";
        ss << " Space         |" << format_score_trace(space_w_s) << "|" << format_score_trace(space_b_s) << "|" << format_score_trace(space_w_s - space_b_s) << "\n";
        ss << " Initiative    |" << "                  |" << "                  |" << format_score_trace(initiative_s) << "\n";
        ss << "---------------+------------------+------------------+--------------------\n";

        // Tính tổng White và Black từ các thành phần đã tách (ĐÃ SỬA)
        SF::Score total_white_components = psqt_w_s + pawns_w_s + mobility_pieces_w_s + king_w_s + threat_w_s + passed_w_s + space_w_s;
        SF::Score total_black_components = psqt_b_s + pawns_b_s + mobility_pieces_b_s + king_b_s + threat_b_s + passed_b_s + space_b_s; // Đã sửa thành + pawns_b_s

        // Tính tổng hiệu số từ các thành phần (không bao gồm imbalance, initiative)
        SF::Score components_diff = total_white_components - total_black_components;

        ss << " COMPONENT SUM |" << format_score_trace(total_white_components) << "|" << format_score_trace(total_black_components) << "|" << format_score_trace(components_diff) << "\n";
        ss << " (+Imbalance)  |" << "                  |" << "                  |" << format_score_trace(components_diff + imbalance_s) << "\n";
        // score_accumulator bây giờ đã bao gồm initiative
        ss << " (+Initiative) |" << "                  |" << "                  |" << format_score_trace(score_accumulator) << "\n"; // Tổng cuối cùng trước nội suy

        // In điểm cuối cùng
        ss << "\nTotal evaluation: " << final_value_white_pov << " (white side)\n";

        return ss.str();
    }


     // --- Hàm In Thông Tin Debug (Đã sửa) ---
    void print_debug_eval_info(const S_BOARD* pos) {
        assert(pos != nullptr);

        std::cout << "\n===== PRINTING DEBUG EVAL INFO =====\n" << std::endl;
        PrintBoard(pos); // In bàn cờ hiện tại

        // 1. Lấy dữ liệu EvalInfo và KingShelter/Storm
        PawnEntry* pe = probe_pawn_table(pos);
        MaterialEntry* me = probe_material_table(pos);
        if (!pe || !me) {
             std::cout << "Error probing tables!" << std::endl;
             std::cout << std::flush; // Đảm bảo lỗi được in ra
             return;
        }
        EvalInfo::Info ei(pos, pe, me);

        SF::Score shelter_w = CalculateKingShelterAndStorm(pos, SF::WHITE);
        SF::Score shelter_b = CalculateKingShelterAndStorm(pos, SF::BLACK);

        // 2. In ra các giá trị
        std::string prefix = "  "; // Tiền tố cho dễ nhìn
        // Mảng tên quân (cần khớp với enum SF::PieceType)
        std::vector<std::string> piece_names = {"NO", "P", "N", "B", "R", "Q", "K", "ALL"};

        std::cout << "\n--- EvalInfo Data ---" << std::endl;

        // King Attackers Count/Weight
        std::cout << prefix << "KingAttackersCount[WHITE] : " << ei.kingAttackersCount[SF::WHITE] << std::endl;
        std::cout << prefix << "KingAttackersCount[BLACK] : " << ei.kingAttackersCount[SF::BLACK] << std::endl;
        std::cout << prefix << "KingAttackersWeight[WHITE]: " << ei.kingAttackersWeight[SF::WHITE] << std::endl;
        std::cout << prefix << "KingAttackersWeight[BLACK]: " << ei.kingAttackersWeight[SF::BLACK] << std::endl;

        // King Ring
        std::cout << prefix << "KingRing[WHITE]:" << std::endl; PrintBitBoard(ei.kingRing[SF::WHITE]);
        std::cout << prefix << "KingRing[BLACK]:" << std::endl; PrintBitBoard(ei.kingRing[SF::BLACK]);

        // Mobility Area
        std::cout << prefix << "MobilityArea[WHITE]:" << std::endl; PrintBitBoard(ei.mobilityArea[SF::WHITE]);
        std::cout << prefix << "MobilityArea[BLACK]:" << std::endl; PrintBitBoard(ei.mobilityArea[SF::BLACK]);

        // AttackedBy2
        std::cout << prefix << "AttackedBy2[WHITE]:" << std::endl; PrintBitBoard(ei.attackedBy2[SF::WHITE]);
        std::cout << prefix << "AttackedBy2[BLACK]:" << std::endl; PrintBitBoard(ei.attackedBy2[SF::BLACK]);

        // AttackedBy cho từng loại quân (ĐÃ THÊM DEBUG PRINT)
         // AttackedBy cho từng loại quân (ĐÃ SỬA LẠI VÒNG LẶP)
         for (SF::Color c : {SF::WHITE, SF::BLACK}) {
            std::string color_str = (c == SF::WHITE) ? "WHITE" : "BLACK";
            std::cout << prefix << "--- AttackedBy[" << color_str << "] ---" << std::endl;

            // 1. Lặp qua các loại quân thực tế (PAWN đến KING)
            for (SF::PieceType pt = SF::PAWN; pt <= SF::KING; ++pt) { // *** SỬA: Lặp đến KING ***
                if (pt >= piece_names.size()) { // Kiểm tra an toàn index
                    std::cout << prefix << "  [INVALID_PIECE_TYPE:" << static_cast<int>(pt) << "]" << std::endl;
                    continue;
                }
                std::cout << prefix << "  [" << piece_names[pt] << "]:" << std::endl; // In tên quân
                PrintBitBoard(ei.attackedBy[c][pt]); // In bitboard của quân đó
                std::cout << std::flush;
            }

            // 2. In riêng phần tổng hợp (Aggregate) được lưu ở index SF::ALL_PIECES (là 0)
            SF::PieceType aggregate_index = SF::ALL_PIECES; // Index 0
            // Đảm bảo tên "ALL" hoặc tương tự có trong mảng piece_names tại index 0
            if (aggregate_index < piece_names.size() && aggregate_index == 0) { // Chỉ in nếu index hợp lệ và là 0
                // Giả sử piece_names[0] là "ALL" hoặc tên phù hợp cho aggregate
                // Nếu piece_names[0] của bạn là "NO", hãy đổi tên hoặc cách truy cập
                // Ví dụ dùng trực tiếp tên:
                std::cout << prefix << "  [ALL]: (Aggregate)" << std::endl;
                PrintBitBoard(ei.attackedBy[c][aggregate_index]); // In bitboard tổng hợp
                std::cout << std::flush;
            } else {
                 // Trường hợp phòng hờ nếu index ALL_PIECES không phải là 0 hoặc piece_names không đúng
                std::cout << prefix << "  [Aggregate - Index/Name Error?]:" << std::endl;
                PrintBitBoard(ei.attackedBy[c][aggregate_index]);
                std::cout << std::flush;
            }
       }

        std::cout << "\n--- KingShelterAndStorm Data ---" << std::endl;
        std::cout << prefix << "Shelter/Storm [WHITE]: " << format_score_trace(shelter_w) << std::endl;
        std::cout << prefix << "Shelter/Storm [BLACK]: " << format_score_trace(shelter_b) << std::endl;

        std::cout << "\n===== END DEBUG EVAL INFO =====\n" << std::endl;
        std::cout << std::flush; // Đảm bảo toàn bộ output được đẩy ra cuối cùng
    }

} // namespace EvalPhase3