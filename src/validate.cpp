#include "validate.h"
#include "init.h"
#include "evaluate.h"

#include <cstring>

bool SqOnBoard(const int sq) { //checks if the square (which is supposed to be part of the 64 square array) is actually one of those
	return FilesBrd[sq] == OFFBOARD ? FALSE : TRUE;
}

bool SideValid(const int side) { // same for the side
	return (side == WHITE || side == BLACK) ? TRUE : FALSE;
}

bool FileRankValid(const int fr) { // for file and rank
	return (fr >= 0 && fr <= 7) ? TRUE : FALSE;
}

bool PieceValidEmpty(const int pce) { // basic, but it just says that any piece is more valuable than an empty square and less than the king (useless for a human but necessary for a computer)
	return (pce >= EMPTY && pce <= bK) ? TRUE : FALSE;
}

bool PieceValid(const int pce) { // same but wiuthout the empty
	return (pce >= wP && pce <= bK) ? TRUE : FALSE;
}

bool MoveListOk(const S_MOVELIST *list, const S_BOARD *pos) {
	if (list->count < 0 || list->count >= MAXPOSITIONMOVES) {
		return FALSE;
	}

	for (int MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		int to = TOSQ(list->moves[MoveNum].move);
		int from = FROMSQ(list->moves[MoveNum].move);
		if (!SqOnBoard(to) || !SqOnBoard(from)) {
			return FALSE;
		}
		if (!PieceValid(pos->pieces[from])) {
			PrintBoard(pos);
			return FALSE;
		}
	}

	return TRUE;
}

void MirrorEvalTest(S_BOARD *pos) {
    FILE *file;
    file = fopen("mirror.epd","r");
    char lineIn [1024];
    int ev1 = 0; int ev2 = 0;
    int positions = 0;
    if(file == NULL) {
        std::cout << "mirrortest 404\n";
        printf("File Not Found\n");
        return;
    }  else {
        std::cout << "mirrortest ok\n";
        while(fgets (lineIn , 1024 , file) != NULL) {
            ParseFen(lineIn, pos);
            positions++;
            ev1 = EvalPosition(pos);
            MirrorBoard(pos);
            ev2 = EvalPosition(pos);

            if(ev1 != ev2) {
                printf("\n\n\n");
                ParseFen(lineIn, pos);
                PrintBoard(pos);
                MirrorBoard(pos);
                PrintBoard(pos);
                printf("\n\nMirror Fail:\n%s\n",lineIn);
                getchar();
                return;
            }

            if( (positions % 10) == 0)  {
                printf("position %d\n",positions);
            }

            memset(&lineIn[0], 0, sizeof(lineIn));
        }
    }
}