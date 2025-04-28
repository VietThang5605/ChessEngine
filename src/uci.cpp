#include "uci.h"
#include "io.h"
#include "makemove.h"
#include "misc.h"

// go depth 6 wtime 180000 btime 100000 binc 1000 winc 1000 movetime 1000 movestogo 4
void ParseGo(char* line, S_SEARCHINFO *info, S_BOARD *pos) {
    int depth = -1, movestogo = 30,movetime = -1;
	int time = -1, inc = 0;
    char *ptr = NULL;
	info->timeSet = FALSE;

	if ((ptr = strstr(line,"infinite"))) {
		;
	}

	if ((ptr = strstr(line,"binc")) && pos->side == BLACK) {
		inc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line,"winc")) && pos->side == WHITE) {
		inc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line,"wtime")) && pos->side == WHITE) {
		time = atoi(ptr + 6);
	}

	if ((ptr = strstr(line,"btime")) && pos->side == BLACK) {
		time = atoi(ptr + 6);
	}

	if ((ptr = strstr(line,"movestogo"))) {
		movestogo = atoi(ptr + 10);
	}

	if ((ptr = strstr(line,"movetime"))) {
		movetime = atoi(ptr + 9);
	}

	if ((ptr = strstr(line,"depth"))) {
		depth = atoi(ptr + 6);
	}

	if(movetime != -1) {
		time = movetime;
		movestogo = 1;
	}

	info->startTime = GetTimeMs();
	info->depth = depth;

	if(time != -1) {
		info->timeSet = TRUE;
		time /= movestogo;
		time -= 50;
		info->stopTime = info->startTime + time + inc;
	}

	if(depth == -1) {
		info->depth = MAXDEPTH;
	}

	printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
		time,info->startTime,info->stopTime,info->depth,info->timeSet);
	SearchPosition(pos, info);
}

void ParsePosition(char* lineIn, S_BOARD *pos) {
    lineIn += 9;
	char *ptrChar = lineIn;
	if (strncmp(lineIn, "startpos", 8) == 0) {
		ParseFen(START_FEN, pos);
	} else {
		ptrChar = strstr(lineIn, "fen");
		if (ptrChar == NULL) {
			ParseFen(START_FEN, pos);
		} else {
			ptrChar += 4;
			ParseFen(ptrChar, pos);
		}
	}

    ptrChar = strstr(lineIn, "moves");
	int move;

	if (ptrChar != NULL) {
		ptrChar += 6;
		while (*ptrChar) {
			move = ParseMove(ptrChar, pos);
			if (move == NOMOVE) break;
			MakeMove(pos, move);
			pos->ply=0;
			while (*ptrChar && *ptrChar != ' ') ptrChar ++;
			ptrChar++;
		}
	}
	PrintBoard(pos);
}

void Uci_Loop() {
    setbuf(stdin, NULL);
	setbuf(stdout, NULL);

    char line[INPUTBUFFER];
	printf("id name %s\n", NAME);
	printf("id author \n");
	printf("uciok\n");

    S_BOARD pos[1];
    S_SEARCHINFO info[1];

	while (TRUE) {
		memset(&line[0], 0, sizeof(line));
		fflush(stdout);

        if (!std::cin.getline(line, sizeof(line)))
            continue;

		if (!strncmp(line, "isready", 7)) {
			printf("readyok\n");
			continue;
        } else if (!strncmp(line, "position", 8)) {
			ParsePosition(line, pos);
		} else if (!strncmp(line, "ucinewgame", 10)) {
			ParsePosition("position startpos\n", pos);
		} else if (!strncmp(line, "go", 2)) {
			ParseGo(line, info, pos);	
		}
		 else if (!strncmp(line, "quit", 4)) {
			info->quit = TRUE;
			break;
		} else if (!strncmp(line, "uci", 3)) {
			// printf("id name %s\n", NAME);
			printf("id author \n");
			printf("uciok\n");
		}
		 if(info->quit) break;
    }
}