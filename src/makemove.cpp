#include "makemove.h"
#include "validate.h"
#include "attack.h"
#include "bitboards.h"
#include "data.h"
#include "init.h"

#include <cstdio>
#include <cassert>

static void ClearPiece(const int sq, S_BOARD *pos)
{

    ASSERT(SqOnBoard(sq));
    ASSERT(CheckBoard(pos));

    int pce = pos->pieces[sq];

    ASSERT(PieceValid(pce));

    int col = PieceColor[pce];
    int index = 0;
    int t_pceNum = -1;

    ASSERT(SideValid(col));

    HASH_PCE(pce, sq);

    pos->pieces[sq] = EMPTY;
    pos->material[col] -= PieceValue[pce];

    if (PieceIsBig[pce])
    {
        pos->bigPiece[col]--;
        if (PieceIsMajor[pce])
        {
            pos->majorPiece[col]--;
        }
        else
        {
            pos->minorPiece[col]--;
        }
    }
    else
    {
        CLRBIT(&pos->pawnsBB[col], SQ64(sq));
        CLRBIT(&pos->pawnsBB[BOTH], SQ64(sq));
    }

    for (index = 0; index < pos->pieceNum[pce]; ++index)
    {
        if (pos->pieceList[pce][index] == sq)
        {
            t_pceNum = index;
            break;
        }
    }

    ASSERT(t_pceNum != -1);
    ASSERT(t_pceNum >= 0 && t_pceNum < 10);

    pos->pieceNum[pce]--;

    pos->pieceList[pce][t_pceNum] = pos->pieceList[pce][pos->pieceNum[pce]];
}

static void AddPiece(const int sq, S_BOARD *pos, const int pce)
{

    ASSERT(PieceValid(pce));
    ASSERT(SqOnBoard(sq));

    int col = PieceColor[pce];
    ASSERT(SideValid(col));

    HASH_PCE(pce, sq);

    pos->pieces[sq] = pce;

    if (PieceIsBig[pce])
    {
        pos->bigPiece[col]++;
        if (PieceIsMajor[pce])
        {
            pos->majorPiece[col]++;
        }
        else
        {
            pos->minorPiece[col]++;
        }
    }
    else
    {
        SETBIT(&pos->pawnsBB[col], SQ64(sq));
        SETBIT(&pos->pawnsBB[BOTH], SQ64(sq));
    }

    pos->material[col] += PieceValue[pce];
    pos->pieceList[pce][pos->pieceNum[pce]++] = sq;
}

static void MovePiece(const int from, const int to, S_BOARD *pos)
{

    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));

    int index = 0;
    int pce = pos->pieces[from];
    int col = PieceColor[pce];
    ASSERT(SideValid(col));
    ASSERT(PieceValid(pce));

#ifdef DEBUG
    int t_PieceNum = FALSE;
#endif

    HASH_PCE(pce, from);
    pos->pieces[from] = EMPTY;

    HASH_PCE(pce, to);
    pos->pieces[to] = pce;

    if (!PieceIsBig[pce])
    {
        CLRBIT(&pos->pawnsBB[col], SQ64(from));
        CLRBIT(&pos->pawnsBB[BOTH], SQ64(from));
        SETBIT(&pos->pawnsBB[col], SQ64(to));
        SETBIT(&pos->pawnsBB[BOTH], SQ64(to));
    }

    for (index = 0; index < pos->pieceNum[pce]; ++index)
    {
        if (pos->pieceList[pce][index] == from)
        {
            pos->pieceList[pce][index] = to;
#ifdef DEBUG
            t_PieceNum = TRUE;
#endif
            break;
        }
    }
    ASSERT(t_PieceNum);
}

int MakeMove(S_BOARD *pos, int move)
{

    ASSERT(CheckBoard(pos));

    int from = FROMSQ(move);
    int to = TOSQ(move);
    int side = pos->side;

    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    ASSERT(SideValid(side));
    ASSERT(PieceValid(pos->pieces[from]));
    ASSERT(pos->hisPly >= 0 && pos->hisPly < MAXGAMEMOVES);
    ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);

    pos->history[pos->hisPly].posKey = pos->posKey;

    if (move & MFLAGEP)
    {
        if (side == WHITE)
        {
            ClearPiece(to - 10, pos);
        }
        else
        {
            ClearPiece(to + 10, pos);
        }
    }
    else if (move & MFLAGCA)
    {
        switch (to)
        {
        case C1:
            MovePiece(A1, D1, pos);
            break;
        case C8:
            MovePiece(A8, D8, pos);
            break;
        case G1:
            MovePiece(H1, F1, pos);
            break;
        case G8:
            MovePiece(H8, F8, pos);
            break;
        default:
            ASSERT(FALSE);
            break;
        }
    }

    if (pos->enPas != NO_SQ)
        HASH_EP;
    HASH_CA;

    pos->history[pos->hisPly].move = move;
    pos->history[pos->hisPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->hisPly].enPas = pos->enPas;
    pos->history[pos->hisPly].castlePerm = pos->castlePerm;

    pos->castlePerm &= CastlePerm[from];
    pos->castlePerm &= CastlePerm[to];
    pos->enPas = NO_SQ;

    HASH_CA;

    int captured = CAPTURED(move);
    pos->fiftyMove++;

    if (captured != EMPTY)
    {
        ASSERT(PieceValid(captured));
        ClearPiece(to, pos);
        pos->fiftyMove = 0;
    }

    pos->hisPly++;
    pos->ply++;

    ASSERT(pos->hisPly >= 0 && pos->hisPly < MAXGAMEMOVES);
    ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);

    if (PieceIsPawn[pos->pieces[from]])
    {
        pos->fiftyMove = 0;
        if (move & MFLAGPS)
        {
            if (side == WHITE)
            {
                pos->enPas = from + 10;
                ASSERT(RanksBrd[pos->enPas] == RANK_3);
            }
            else
            {
                pos->enPas = from - 10;
                ASSERT(RanksBrd[pos->enPas] == RANK_6);
            }
            HASH_EP;
        }
    }

    MovePiece(from, to, pos);

    int prPce = PROMOTED(move);
    if (prPce != EMPTY)
    {
        ASSERT(PieceValid(prPce) && !PieceIsPawn[prPce]);
        ClearPiece(to, pos);
        AddPiece(to, pos, prPce);
    }

    if (PieceIsKing[pos->pieces[to]])
    {
        pos->kingSquare[pos->side] = to;
    }

    pos->side ^= 1;
    HASH_SIDE;

    ASSERT(CheckBoard(pos));

    if (SqAttacked(pos->kingSquare[side], pos->side, pos))
    {
        TakeMove(pos);
        return FALSE;
    }

    return TRUE;
}

void TakeMove(S_BOARD *pos)
{

    ASSERT(CheckBoard(pos));

    pos->hisPly--;
    pos->ply--;

    ASSERT(pos->hisPly >= 0 && pos->hisPly < MAXGAMEMOVES);
    ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);

    int move = pos->history[pos->hisPly].move;
    int from = FROMSQ(move);
    int to = TOSQ(move);

    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));

    if (pos->enPas != NO_SQ)
        HASH_EP;
    HASH_CA;

    pos->castlePerm = pos->history[pos->hisPly].castlePerm;
    pos->fiftyMove = pos->history[pos->hisPly].fiftyMove;
    pos->enPas = pos->history[pos->hisPly].enPas;

    if (pos->enPas != NO_SQ)
        HASH_EP;
    HASH_CA;

    pos->side ^= 1;
    HASH_SIDE;

    if (MFLAGEP & move)
    {
        if (pos->side == WHITE)
        {
            AddPiece(to - 10, pos, bP);
        }
        else
        {
            AddPiece(to + 10, pos, wP);
        }
    }
    else if (MFLAGCA & move)
    {
        switch (to)
        {
        case C1:
            MovePiece(D1, A1, pos);
            break;
        case C8:
            MovePiece(D8, A8, pos);
            break;
        case G1:
            MovePiece(F1, H1, pos);
            break;
        case G8:
            MovePiece(F8, H8, pos);
            break;
        default:
            ASSERT(FALSE);
            break;
        }
    }

    MovePiece(to, from, pos);

    if (PieceIsKing[pos->pieces[from]])
    {
        pos->kingSquare[pos->side] = from;
    }

    int captured = CAPTURED(move);
    if (captured != EMPTY)
    {
        ASSERT(PieceValid(captured));
        AddPiece(to, pos, captured);
    }

    if (PROMOTED(move) != EMPTY)
    {
        ASSERT(PieceValid(PROMOTED(move)) && !PieceIsPawn[PROMOTED(move)]);
        ClearPiece(from, pos);
        AddPiece(from, pos, (PieceColor[PROMOTED(move)] == WHITE ? wP : bP));
    }

    ASSERT(CheckBoard(pos));
}