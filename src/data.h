#ifndef DATA_H
#define DATA_H

#include "types.h"

#define IsBishopQueen(p) (PieceIsBishopQueen[(p)])
#define IsRookQueen(p) (PieceIsRookQueen[(p)])
#define IsKnight(p) (PieceIsKnight[(p)])
#define IsBishop(p) (PieceIsBishop[(p)])
#define IsRook(p) (PieceIsRook[(p)])
#define IsQueen(p) (PieceIsQueen[(p)])
#define IsKing(p) (PieceIsKing[(p)])

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
#endif