#include "data.h"

char PieceChar[] = ".PNBRQKpnbrqk"; // piece according the number
char SideChar[] = "wb-";            // self explanatory '-' is for both b and w
char RankChar[] = "12345678";       // for ranks
char FileChar[] = "abcdefgh";       // for files

// Định nghĩa & khởi tạo duy nhất ở đây
int PieceBig[13] = {0};
int PieceMaj[13] = {0};
int PieceMin[13] = {0};
int PieceVal[13] = {0};
int PieceCol[13] = {0};
int PiecePawn[13] = {0};
int PieceKing[13] = {0};
int PieceRookQueen[13] = {0};
int PieceBishopQueen[13] = {0};
int PieceKnight[13] = {0};
int FilesBrd[120] = {0};
int RanksBrd[120] = {0};