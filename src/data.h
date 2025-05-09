#ifndef DATA_H
#define DATA_H

#include "types.h"

#define IsBishopQueen(p) (PieceIsBishopQueen[(p)])
#define IsRookQueen(p) (PieceIsRookQueen[(p)])
#define IsPawn(p) (PieceIsPawn[(p)])
#define IsKnight(p) (PieceIsKnight[(p)])
#define IsBishop(p) (PieceIsBishop[(p)])
#define IsRook(p) (PieceIsRook[(p)])
#define IsQueen(p) (PieceIsQueen[(p)])
#define IsKing(p) (PieceIsKing[(p)])

#define MIRROR64(sq) (Mirror64[(sq)])

extern char PieceChar[]; //piece according the number
extern char SideChar[]; //self explanatory '-' is for both b and w
extern char RankChar[]; //for ranks
extern char FileChar[]; //for files

extern bool PieceIsBig[13]; //knight bishop rook queen king are big pieces
extern bool PieceIsMajor[13]; // king queen rook (black and white)
extern bool PieceIsMinor[13]; //knight bishop
extern int PieceValue[13];
extern int PieceColor[13]; //gives colo of a piece

extern bool PieceIsPawn[13];
extern bool PieceIsKnight[13];
extern bool PieceIsBishop[13];
extern bool PieceIsRook[13];
extern bool PieceIsQueen[13];
extern bool PieceIsKing[13];
extern bool PieceIsRookQueen[13];
extern bool PieceIsBishopQueen[13];
extern bool PieceIsSlides[13];

const int Mirror64[64] = {
    56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
    48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
    40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
    32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
    24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
    16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
    8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
    0	,	1	,	2	,	3	,	4	,	5	,	6	,	7		
};
#endif