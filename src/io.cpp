#include "io.h"
#include "validate.h"
#include "movegen.h"
#include "data.h"

#include <cstdio>

int ParseMove(char *ptrChar, S_BOARD *pos)
{

    ASSERT(CheckBoard(pos));

    if (ptrChar[1] > '8' || ptrChar[1] < '1')
        return NOMOVE;
    if (ptrChar[3] > '8' || ptrChar[3] < '1')
        return NOMOVE;
    if (ptrChar[0] > 'h' || ptrChar[0] < 'a')
        return NOMOVE;
    if (ptrChar[2] > 'h' || ptrChar[2] < 'a')
        return NOMOVE;

    int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
    int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

    ASSERT(SqOnBoard(from) && SqOnBoard(to));

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);
    int MoveNum = 0;
    int Move = 0;
    int PromPce = EMPTY;

    for (MoveNum = 0; MoveNum < list->count; ++MoveNum)
    {
        Move = list->moves[MoveNum].move;
        if (FROMSQ(Move) == from && TOSQ(Move) == to)
        {
            PromPce = PROMOTED(Move);
            if (PromPce != EMPTY)
            {
                if (IsRQ(PromPce) && !IsBQ(PromPce) && ptrChar[4] == 'r')
                {
                    return Move;
                }
                else if (!IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'b')
                {
                    return Move;
                }
                else if (IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'q')
                {
                    return Move;
                }
                else if (IsKn(PromPce) && ptrChar[4] == 'n')
                {
                    return Move;
                }
                continue;
            }
            return Move;
        }
    }

    return NOMOVE;
}