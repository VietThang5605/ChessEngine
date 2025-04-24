#include "types.h"

extern char PieceChar[]; //piece according the number
extern char SideChar[]; //self explanatory '-' is for both b and w
extern char RankChar[]; //for ranks
extern char FileChar[]; //for files

extern int PieceIsBig[13]; //knight bishop rook queen king are big pieces
extern int PieceIsMajor[13]; // king queen rook (black and white)
extern int PieceIsMinor[13]; //knight bishop
extern int PieceValue[13];
extern int PieceColor[13]; //gives colo of a piece