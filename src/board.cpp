#include "board.h"
#include "bitboards.h"
#include "validate.h"
#include "data.h"

#include <iostream>
#include <iomanip>

int PceListOk(const S_BOARD *pos)
{
    int pce = wP;
    int sq;
    int num;
    for (pce = wP; pce <= bK; ++pce)
    {
        if (pos->pieceNum[pce] < 0 || pos->pieceNum[pce] >= 10)
            return FALSE;
    }

    if (pos->pieceNum[wK] != 1 || pos->pieceNum[bK] != 1)
        return FALSE;

    for (pce = wP; pce <= bK; ++pce)
    {
        for (num = 0; num < pos->pieceNum[pce]; ++num)
        {
            sq = pos->pieceList[pce][num];
            if (!SqOnBoard(sq))
                return FALSE;
        }
    }
    return TRUE;
}

int CheckBoard(const S_BOARD *pos)
{

    int t_pceNum[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int t_bigPce[2] = {0, 0};
    int t_majPce[2] = {0, 0};
    int t_minPce[2] = {0, 0};
    int t_material[2] = {0, 0};

    int sq64, t_piece, t_pce_num, sq120, colour, pcount;

    U64 t_pawns[3] = {0ULL, 0ULL, 0ULL};

    t_pawns[WHITE] = pos->pawnsBB[WHITE];
    t_pawns[BLACK] = pos->pawnsBB[BLACK];
    t_pawns[BOTH] = pos->pawnsBB[BOTH];

    // check piece lists
    for (t_piece = wP; t_piece <= bK; ++t_piece)
    {
        for (t_pce_num = 0; t_pce_num < pos->pieceNum[t_piece]; ++t_pce_num)
        {
            sq120 = pos->pieceList[t_piece][t_pce_num];
            ASSERT(pos->pieces[sq120] == t_piece);
        }
    }

    // check piece count and other counters
    for (sq64 = 0; sq64 < 64; ++sq64)
    {
        sq120 = SQ120(sq64);
        t_piece = pos->pieces[sq120];
        t_pceNum[t_piece]++;
        colour = PieceCol[t_piece];
        if (PieceBig[t_piece] == TRUE)
            t_bigPce[colour]++;
        if (PieceMin[t_piece] == TRUE)
            t_minPce[colour]++;
        if (PieceMaj[t_piece] == TRUE)
            t_majPce[colour]++;

        t_material[colour] += PieceVal[t_piece];
    }

    for (t_piece = wP; t_piece <= bK; ++t_piece)
    {
        ASSERT(t_pceNum[t_piece] == pos->pieceNum[t_piece]);
    }

    // check bitboards count
    pcount = CNT(t_pawns[WHITE]);
    ASSERT(pcount == pos->pieceNum[wP]);
    pcount = CNT(t_pawns[BLACK]);
    ASSERT(pcount == pos->pieceNum[bP]);
    pcount = CNT(t_pawns[BOTH]);
    ASSERT(pcount == (pos->pieceNum[bP] + pos->pieceNum[wP]));

    // check bitboards squares
    while (t_pawns[WHITE])
    {
        sq64 = POP(&t_pawns[WHITE]);
        ASSERT(pos->pieces[SQ120(sq64)] == wP);
    }

    while (t_pawns[BLACK])
    {
        sq64 = POP(&t_pawns[BLACK]);
        ASSERT(pos->pieces[SQ120(sq64)] == bP);
    }

    while (t_pawns[BOTH])
    {
        sq64 = POP(&t_pawns[BOTH]);
        ASSERT((pos->pieces[SQ120(sq64)] == bP) || (pos->pieces[SQ120(sq64)] == wP));
    }

    ASSERT(t_material[WHITE] == pos->material[WHITE] && t_material[BLACK] == pos->material[BLACK]);
    ASSERT(t_minPce[WHITE] == pos->minPiece[WHITE] && t_minPce[BLACK] == pos->minPiece[BLACK]);
    ASSERT(t_majPce[WHITE] == pos->majPiece[WHITE] && t_majPce[BLACK] == pos->majPiece[BLACK]);
    ASSERT(t_bigPce[WHITE] == pos->bigPiece[WHITE] && t_bigPce[BLACK] == pos->bigPiece[BLACK]);

    ASSERT(pos->side == WHITE || pos->side == BLACK);
    ASSERT(GeneratePosKey(pos) == pos->posKey);

    ASSERT(pos->enPas == NO_SQ || (RanksBrd[pos->enPas] == RANK_6 && pos->side == WHITE) || (RanksBrd[pos->enPas] == RANK_3 && pos->side == BLACK));

    ASSERT(pos->pieces[pos->KingSq[WHITE]] == wK);
    ASSERT(pos->pieces[pos->KingSq[BLACK]] == bK);

    ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);

    ASSERT(PceListOk(pos));

    return TRUE;
}

int ParseFen(char *fen, S_BOARD *pos)
{
    ASSERT(fen != NULL);
    ASSERT(pos != NULL);

    Rank rank = RANK_8;
    File file = FILE_A;

    ResetBoard(pos);

    while ((rank >= RANK_1) && *fen)
    {
        int piece = 0;
        int count = 1;

        switch (*fen)
        {
        case 'p':
            piece = bP;
            break;
        case 'r':
            piece = bR;
            break;
        case 'n':
            piece = bN;
            break;
        case 'b':
            piece = bB;
            break;
        case 'k':
            piece = bK;
            break;
        case 'q':
            piece = bQ;
            break;
        case 'P':
            piece = wP;
            break;
        case 'R':
            piece = wR;
            break;
        case 'N':
            piece = wN;
            break;
        case 'B':
            piece = wB;
            break;
        case 'K':
            piece = wK;
            break;
        case 'Q':
            piece = wQ;
            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            piece = EMPTY;
            count = *fen - '0';
            break;

        case '/':
        case ' ':
            --rank;
            file = FILE_A;
            fen++;
            continue;

        default:
            std::cerr << "FEN error \n";
            return -1;
        }

        for (int i = 0; i < count; ++i)
        {
            int sq64 = rank * 8 + file;
            int sq120 = SQ120(sq64);
            if (piece != EMPTY)
            {
                pos->pieces[sq120] = piece;
            }
            ++file;
        }
        fen++;
    }

    ASSERT(*fen == 'w' || *fen == 'b');
    pos->side = (*fen == 'w') ? WHITE : BLACK;
    fen += 2; // move to enPas

    for (int i = 0; i < 4; ++i)
    {
        if (*fen == ' ')
        {
            break;
        }
        switch (*fen)
        {
        case 'K':
            pos->castlePerm |= WKCA;
            break;
        case 'Q':
            pos->castlePerm |= WQCA;
            break;
        case 'k':
            pos->castlePerm |= BKCA;
            break;
        case 'q':
            pos->castlePerm |= BQCA;
            break;
        default:
            break;
        }
        fen++;
    }
    fen++;

    ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);

    if (*fen != '-')
    {
        file = File(fen[0] - 'a');
        rank = Rank(fen[1] - '1');
        ASSERT(file >= FILE_A && file <= FILE_H);
        ASSERT(rank >= RANK_1 && rank <= RANK_8);
        pos->enPas = FR2SQ(file, rank);
    }

    pos->posKey = GeneratePosKey(pos);

    return 0;
}

void ResetBoard(S_BOARD *pos)
{
    for (int i = 0; i < BRD_SQ_NUM; ++i)
    {                              // looping through all 120 squares
        pos->pieces[i] = OFFBOARD; // and setting them to "OFFBOARD"
    } // for more understanding, look at the image in the folder named "board representation"

    for (int i = 0; i < SQUARE_NB; ++i)
    {
        pos->pieces[SQ120(i)] = EMPTY; // then the "real" squares are set to "empty"
    }

    for (int i = 0; i < 2; ++i)
    {
        pos->bigPiece[i] = 0; // number of all pieces (black and white)
        pos->majPiece[i] = 0;
        pos->minPiece[i] = 0;
        pos->material[i] = 0; // set material scores to 0
    }

    for (int i = 0; i < 3; i++)
    {
        pos->pawnsBB[i] = 0ULL; // OULL cuz pawnsBB are an U64 type in an unsigned long long
        pos->knightsBB[i] = 0ULL;
        pos->bishopsBB[i] = 0ULL;
        pos->rooksBB[i] = 0ULL;
        pos->queensBB[i] = 0ULL;
        pos->kingsBB[i] = 0ULL;
    }

    for (int i = 0; i < 13; ++i)
    {
        pos->pieceNum[i] = 0; // resets the piece number
    }

    pos->KingSq[WHITE] = pos->KingSq[BLACK] = NO_SQ;

    pos->side = BOTH; // once we setup a position, checkboard will check some things so we set it to both not to get an ASSERT
    pos->enPas = NO_SQ;
    pos->fiftyMove = 0;

    pos->ply = 0; // nber of half moves played in the current search
    pos->hisPly = 0;

    pos->castlePerm = 0;

    pos->posKey = 0ULL;
}

void PrintBoard(const S_BOARD *pos)
{

    std::cout << "\nGame Board:\n\n";

    for (Rank rank = RANK_8; rank >= RANK_1; --rank)
    { // stat on the 8th rank cuz its how we look at it
        printf("%d  ", rank + 1);
        for (File file = FILE_A; file <= FILE_H; ++file)
        {
            int sq = FR2SQ(file, rank);  // sqare index
            int piece = pos->pieces[sq]; // get the piece using the square index from board
            std::cout << std::setw(3) << PieceChar[piece];
        }
        std::cout << '\n';
    }

    std::cout << "\n   ";
    for (File file = FILE_A; file <= FILE_H; ++file)
    {
        std::cout << std::setw(3) << char('a' + file);
    }
    std::cout << '\n';

    std::cout << "side: " << SideChar[pos->side] << '\n';
    std::cout << "enPas: " << std::dec << pos->enPas << '\n'; // printed as a decimal rather then as characters
    std::cout << "castle: "
              << (pos->castlePerm & WKCA ? 'K' : '-') // if castle perm ends with ' ' then print the corresponding letters
              << (pos->castlePerm & WQCA ? 'Q' : '-')
              << (pos->castlePerm & BKCA ? 'k' : '-')
              << (pos->castlePerm & BQCA ? 'q' : '-')
              << '\n';
    std::cout << "PosKey: ";
    std::cout << std::hex << std::uppercase << pos->posKey << '\n'; // hexa cuz easier to read
}