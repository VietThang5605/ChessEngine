#include <algorithm>
#include <cassert>
#include <cstring>   // For std::memset
#include <iomanip>
#include <sstream>

#include "evaluation.h"

namespace Trace {

  enum Tracing { NO_TRACE, TRACE };

  enum Term { // The first 8 entries are reserved for PieceType
    MATERIAL = 8, IMBALANCE, MOBILITY, THREAT, PASSED, SPACE, INITIATIVE, TOTAL, TERM_NB
  };

  SF::Score scores[TERM_NB][COLOR_NB];

  double to_cp(SF::Value v) { return double(v) / SF::PawnValueEg; }

  void add(int idx, SF::Color c, SF::Score s) {
    scores[idx][c] = s;
  }

  void add(int idx, SF::Score w, SF::Score b = SF::SCORE_ZERO) {
    scores[idx][WHITE] = w;
    scores[idx][BLACK] = b;
  }

  std::ostream& operator<<(std::ostream& os, SF::Score s) {
    os << std::setw(5) << to_cp(SF::mg_value(s)) << " "
       << std::setw(5) << to_cp(SF::eg_value(s));
    return os;
  }

  std::ostream& operator<<(std::ostream& os, Term t) {

    if (t == MATERIAL || t == IMBALANCE || t == INITIATIVE || t == TOTAL)
        os << " ----  ----"    << " | " << " ----  ----";
    else
        os << scores[t][WHITE] << " | " << scores[t][BLACK];

    os << " | " << scores[t][WHITE] - scores[t][BLACK] << "\n";
    return os;
  }
}

using namespace Trace;

namespace {

  // Threshold for lazy and space evaluation
  constexpr SF::Value LazyThreshold  = SF::Value(1400);
  constexpr SF::Value SpaceThreshold = SF::Value(12222);

  // KingAttackWeights[PieceType] contains king attack weights by piece type
  constexpr int KingAttackWeights[PIECE_TYPE_NB] = { 0, 0, 81, 52, 44, 10 };

  // Penalties for enemy's safe checks
  constexpr int QueenSafeCheck  = 780;
  constexpr int RookSafeCheck   = 1080;
  constexpr int BishopSafeCheck = 635;
  constexpr int KnightSafeCheck = 790;

#define S(mg, eg) SF::make_score(mg, eg)

  // MobilityBonus[PieceType-2][attacked] contains bonuses for middle and end game,
  // indexed by piece type and number of attacked squares in the mobility area.
  constexpr SF::Score MobilityBonus[][32] = {
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

  // RookOnFile[semiopen/open] contains bonuses for each rook when there is
  // no (friendly) pawn on the rook file.
  constexpr SF::Score RookOnFile[] = { S(21, 4), S(47, 25) };

  // ThreatByMinor/ByRook[attacked PieceType] contains bonuses according to
  // which piece type attacks which one. Attacks on lesser pieces which are
  // pawn-defended are not considered.
  constexpr SF::Score ThreatByMinor[PIECE_TYPE_NB] = {
    S(0, 0), S(6, 32), S(59, 41), S(79, 56), S(90, 119), S(79, 161)
  };

  constexpr SF::Score ThreatByRook[PIECE_TYPE_NB] = {
    S(0, 0), S(3, 44), S(38, 71), S(38, 61), S(0, 38), S(51, 38)
  };

  // PassedRank[Rank] contains a bonus according to the rank of a passed pawn
  constexpr SF::Score PassedRank[RANK_NB] = {
    S(0, 0), S(10, 28), S(17, 33), S(15, 41), S(62, 72), S(168, 177), S(276, 260)
  };

  // Assorted bonuses and penalties
  constexpr SF::Score BishopPawns        = S(  3,  7);
  constexpr SF::Score CorneredBishop     = S( 50, 50);
  constexpr SF::Score FlankAttacks       = S(  8,  0);
  constexpr SF::Score Hanging            = S( 69, 36);
  constexpr SF::Score KingProtector      = S(  7,  8);
  constexpr SF::Score KnightOnQueen      = S( 16, 12);
  constexpr SF::Score LongDiagonalBishop = S( 45,  0);
  constexpr SF::Score MinorBehindPawn    = S( 18,  3);
  constexpr SF::Score Outpost            = S( 30, 21);
  constexpr SF::Score PassedFile         = S( 11,  8);
  constexpr SF::Score PawnlessFlank      = S( 17, 95);
  constexpr SF::Score RestrictedPiece    = S(  7,  7);
  constexpr SF::Score ReachableOutpost   = S( 32, 10);
  constexpr SF::Score RookOnQueenFile    = S(  7,  6);
  constexpr SF::Score SliderOnQueen      = S( 59, 18);
  constexpr SF::Score ThreatByKing       = S( 24, 89);
  constexpr SF::Score ThreatByPawnPush   = S( 48, 39);
  constexpr SF::Score ThreatBySafePawn   = S(173, 94);
  constexpr SF::Score TrappedRook        = S( 52, 10);
  constexpr SF::Score WeakQueen          = S( 49, 15);

#undef S

}// namespace close
    
