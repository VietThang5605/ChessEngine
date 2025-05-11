#include "tbprobe.h"

// Given a position with 6 or fewer pieces, produce a text string
// of the form KQPvKRP, where "KQP" represents the white pieces if
// mirror == 0 and the black pieces if mirror == 1.
// No need to make this very efficient.
static void prt_str(Position& pos, char *str, int mirror)
{
  Color color;
  PieceType pt;
  int i;
  
  color = !mirror ? WHITE : BLACK;
  for (pt = KING; pt >= PAWN; pt--)
    for (i = popcount<Max15>(pos.pieces(color, pt)); i > 0; i--)
      *str++ = pchr[6 - pt];
  *str++ = 'v';
  color = ~color;
  for (pt = KING; pt >= PAWN; pt--)
    for (i = popcount<Max15>(pos.pieces(color, pt)); i > 0; i--)
      *str++ = pchr[6 - pt];
  *str++ = 0;
}