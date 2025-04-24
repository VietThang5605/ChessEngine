#include "data.h"

char PieceChar[] = ".PNBRQKpnbrqk"; //piece according the number
char SideChar[] = "wb-"; //self explanatory '-' is for both b and w
char RankChar[] = "12345678"; //for ranks
char FileChar[] = "abcdefgh"; //for files

int PieceIsBig[13] = { FALSE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE };
int PieceIsMajor[13] = { FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE };
int PieceIsMinor[13] = { FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE };
int PieceValue[13]= { 0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000  }; // pawn worth 100 and 325 for bishop and knight rather than 300 cuz "oldschool"
int PieceColor[13] = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
	                BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };

int PieceIsPawn[13] = { FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE };	
int PieceIsKnight[13] = { FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE };
int PieceIsKing[13] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE };
int PieceIsRookQueen[13] = { FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE };
int PieceIsBishopQueen[13] = { FALSE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE };
int PieceIsSlides[13] = { FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE };