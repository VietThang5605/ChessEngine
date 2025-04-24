#include "attack.h"
#include "validate.h"
#include "data.h"

#include <cstdio>

int SqAttacked(const int sq, const int side, const S_BOARD *pos)
{

    int pce, index, t_sq, dir;

    ASSERT(SqOnBoard(sq));
    ASSERT(SideValid(side));
    ASSERT(CheckBoard(pos));

    // pawns
    if (side == WHITE)
    {
        if (pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP)
        {
            return TRUE;
        }
    }
    else
    {
        if (pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP)
        {
            return TRUE;
        }
    }

    // knights
    for (index = 0; index < 8; ++index)
    {
        pce = pos->pieces[sq + KnDir[index]];
        ASSERT(PceValidEmptyOffbrd(pce));
        if (pce != OFFBOARD && IsKn(pce) && PieceCol[pce] == side)
        {
            return TRUE;
        }
    }

    // rooks, queens
    for (index = 0; index < 4; ++index)
    {
        dir = RkDir[index];
        t_sq = sq + dir;
        ASSERT(SqIs120(t_sq));
        pce = pos->pieces[t_sq];
        ASSERT(PceValidEmptyOffbrd(pce));
        while (pce != OFFBOARD)
        {
            if (pce != EMPTY)
            {
                if (IsRQ(pce) && PieceCol[pce] == side)
                {
                    return TRUE;
                }
                break;
            }
            t_sq += dir;
            ASSERT(SqIs120(t_sq));
            pce = pos->pieces[t_sq];
        }
    }

    // bishops, queens
    for (index = 0; index < 4; ++index)
    {
        dir = BiDir[index];
        t_sq = sq + dir;
        ASSERT(SqIs120(t_sq));
        pce = pos->pieces[t_sq];
        ASSERT(PceValidEmptyOffbrd(pce));
        while (pce != OFFBOARD)
        {
            if (pce != EMPTY)
            {
                if (IsBQ(pce) && PieceCol[pce] == side)
                {
                    return TRUE;
                }
                break;
            }
            t_sq += dir;
            ASSERT(SqIs120(t_sq));
            pce = pos->pieces[t_sq];
        }
    }

    // kings
    for (index = 0; index < 8; ++index)
    {
        pce = pos->pieces[sq + KiDir[index]];
        ASSERT(PceValidEmptyOffbrd(pce));
        if (pce != OFFBOARD && IsKi(pce) && PieceCol[pce] == side)
        {
            return TRUE;
        }
    }

    return FALSE;
}
#include "attack.h"

bool SqAttacked(const int sq, const int side, const S_BOARD *pos)
{
    // what square, which side is attacking it and in what position basically

    ASSERT(CheckBoard(pos));

    // pawns
    if (side == WHITE)
    {
        if (pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP)
        {
            return TRUE;
        }
    }
    else
    {
        if (pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP)
        {
            return TRUE;
        }
    }

    // knights
    for (int i = 0; i < 8; ++i)
    {
        int piece = pos->pieces[sq + KnightDir[i]];
        // ASSERT(PceValidEmptyOffbrd(pce));
        // EMPTY is also in PieceIsKinght and it's value is FALSE
        if (piece != OFFBOARD && IsKnight(piece) && PieceColor[piece] == side)
        {
            return TRUE;
        }
    }

    // rooks, queens
    for (int i = 0; i < 4; ++i)
    {
        int dir = RookDir[i];
        int t_sq = sq + dir;
        int piece = pos->pieces[t_sq];
        while (piece != OFFBOARD)
        {
            if (piece != EMPTY)
            {
                if (IsRookQueen(piece) && PieceColor[piece] == side)
                {
                    return TRUE;
                }
                break;
            }
            t_sq += dir;
            piece = pos->pieces[t_sq];
        }
    }

    // bishops, queens
    for (int i = 0; i < 4; ++i)
    {
        int dir = BishopDir[i];
        int t_sq = sq + dir;
        int piece = pos->pieces[t_sq];
        while (piece != OFFBOARD)
        {
            if (piece != EMPTY)
            {
                if (IsBishopQueen(piece) && PieceColor[piece] == side)
                {
                    return TRUE;
                }
                break;
            }
            t_sq += dir;
            piece = pos->pieces[t_sq];
        }
    }

    // kings
    for (int i = 0; i < 8; ++i)
    {
        int piece = pos->pieces[sq + KingDir[i]];
        if (piece != OFFBOARD && IsKing(piece) && PieceColor[piece] == side)
        {
            return TRUE;
        }
    }

    return FALSE;
}