#include "uci.h"
#include "misc.h"
#include "makemove.h"
#include "io.h"
#include "ucioption.h"
#include "pvtable.h"
#include "evaluate.h"
#include "types.h" //must before tinycthread to using it's FALSE
#include "tinycthread.h"
#include "validate.h" //test evaluate

#include <cstring>
#include <iostream>
#include <limits>
#include <math.h>

#define MAXTHINKINGTIMEMS (double)15000

#define INPUTBUFFER 400 * 6

thrd_t mainSearchThread;

thrd_t LaunchSearchThread(S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table) {
	S_SEARCH_THREAD_DATA *pSearchData = new S_SEARCH_THREAD_DATA[1];
	pSearchData->originalPosition = pos;
	pSearchData->info = info;
	pSearchData->ttable = table;

	thrd_t th;
	thrd_create(&th, &SearchPosition_Thread, (void *)pSearchData);

	return th;
}

void JoinSearchThread(S_SEARCHINFO *info) {
	info->stopped = TRUE;
	thrd_join(mainSearchThread, NULL);
}

// go depth 6 wtime 180000 btime 100000 binc 1000 winc 1000 movetime 1000 movestogo 40
void ParseGo(char* line, S_SEARCHINFO *info, S_BOARD *pos, S_HASHTABLE *table) {
	int depth = -1, movestogo = 30, movetime = -1;
	int inc = 0;
	int wtime = -1, btime = -1, binc = 0, winc = 0;
    char *ptr = NULL;
	info->timeSet = FALSE;

	if ((ptr = strstr(line, "infinite"))) {
		;
	}

	if ((ptr = strstr(line, "winc"))) {
		winc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line, "binc"))) {
		binc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line, "wtime"))) {
		wtime = atoi(ptr + 6);
	}

	if ((ptr = strstr(line, "btime"))) {
		btime = atoi(ptr + 6);
	}

	if ((ptr = strstr(line, "movestogo"))) {
		movestogo = atoi(ptr + 10);
	}

	if ((ptr = strstr(line, "movetime"))) {
		movetime = atoi(ptr + 9);
	}

	if ((ptr = strstr(line, "depth"))) {
		depth = atoi(ptr + 6);
	}

	info->startTime = GetTimeMs();
	info->depth = depth;

	if (movetime != -1) {
		info->timeSet = TRUE;
		movetime -= 50;
		info->stopTime = info->startTime + movetime;
		
	} else if ((wtime != -1 && pos->side == WHITE) || (btime != -1 && pos->side == BLACK)) {
		info->timeSet = TRUE;

		int remainingTimeMs = (pos->side == WHITE ? wtime : btime);
		int incrementTimeMs = (pos->side == WHITE ? winc : binc);

		int divisor = (GetTotalPieceNum(pos) > 16 || pos->fullMoveNumber < 20) ? 35 : 40;
		double thinkTimeMs = remainingTimeMs / divisor;
		thinkTimeMs = std::min(MAXTHINKINGTIMEMS, thinkTimeMs);

		if (remainingTimeMs > 2 * incrementTimeMs) {
			thinkTimeMs += incrementTimeMs * 0.8;
		}

		double minThinkingTime = std::min((double)200, remainingTimeMs * 0.25);
		movetime = std::max((int)minThinkingTime, (int)thinkTimeMs);
		movetime -= 50;
		info->stopTime = info->startTime + movetime;

		// std::cout << "Total piece: " << GetTotalPieceNum(pos) << '\n';
		std::cout << "movetime: " << movetime << '\n';
	}

	if (depth == -1) {
		info->depth = MAXDEPTH;
	}

	std::cout << "time:" << movetime << " start:" << info->startTime << " stop:" << info->stopTime 
			<< " depth:" << info->depth << " timeset:" << info->timeSet << '\n';
	// SearchPosition(pos, info, HashTable);
	mainSearchThread =  LaunchSearchThread(pos, info, table);
}

void ParsePosition(char* lineIn, S_BOARD *pos) {
	lineIn += 9;
    char *ptrChar = lineIn;

    if(strncmp(lineIn, "startpos", 8) == 0){
        ParseFen(START_FEN, pos);
    } else {
        ptrChar = strstr(lineIn, "fen");
        if(ptrChar == NULL) {
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
              move = ParseMove(ptrChar,pos);
			  if (move == NOMOVE) break;
			  std::cout << "MOVE: " << PrintMove(move) << '\n';
			  MakeMove(pos, move);
              pos->ply=0;
              while (*ptrChar && *ptrChar!= ' ') ptrChar++;
              ptrChar++;
        }
    }

	PrintBoard(pos);
}

void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info) {
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	char line[INPUTBUFFER];
	std::cout << "id name " << NAME << '\n';
	std::cout << "id author TelietTeam\n";
	printf("option name Hash type spin default 64 min 4 max %d\n", MAX_HASH);
	printf("option name Threads type spin default 1 min 1 max %d\n", MAXTHREAD);
	printf("option name Book type check default true\n");
	std::cout << "uciok\n";

	int MB = 64;

	while (TRUE) {
		memset(&line, '\0', sizeof(line));

		if (!std::cin.getline(line, sizeof(line))) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}
		
		if (line[0] == '\0')
			continue;

		if(!strncmp(line, "isready", 7)) {
			printf("readyok\n");
			continue;

		} else if(!strncmp(line, "position", 8)) {
			ParsePosition(line, pos);

		} else if(!strncmp(line, "ucinewgame", 10)) {
			ClearHashTable(HashTable);
			ParsePosition("position startpos\n", pos);

		} else if(!strncmp(line, "go", 2)) {
			ParseGo(line, info, pos, HashTable);	

		} else if(!strncmp(line, "run", 3)) {
			// ParseFen(START_FEN, pos);
			// ParseFen(WAC_2, pos);
			// ParseFen(FINE_70, pos);
			// ParseFen(LCT_1, pos);
			ParseFen(RRRR, pos);
			ParseGo("go infinite", info, pos, HashTable);	

		} else if(!strncmp(line, "stop", 4)) {
			JoinSearchThread(info);

		} else if(!strncmp(line, "quit", 4)) {
			info->quit = TRUE;
			JoinSearchThread(info);
			break;

		} else if (!strncmp(line, "uci", 3)) {
			std::cout << "id name " << NAME << '\n';
			std::cout << "id author TelietTeam\n";
			std::cout << "uciok\n";

		} else if (!strncmp(line, "setoption name Hash value ", 26)) {			
			sscanf(line,"%*s %*s %*s %*s %d", &MB);
			if (MB < 4) MB = 4;
			if (MB > MAX_HASH) MB = MAX_HASH;
			std::cout << "Set Hash to " << MB << " MB\n";
			InitHashTable(HashTable, MB);

		} else if (!strncmp(line, "setoption name Threads value ", 29)) {			
			sscanf(line,"%*s %*s %*s %*s %d", &MB);
			if (MB < 1) MB = 1;
			if (MB > MAXTHREAD) MB = MAXTHREAD;
			std::cout << "Set Threads to " << MB << " Threads\n";
			info->threadNumber = MB;

		} else if (!strncmp(line, "setoption name Book value ", 26)) {			
			char *ptrTrue = NULL;
			ptrTrue = strstr(line, "true");
			if (ptrTrue != NULL) {
				EngineOptions->UseBook = TRUE;
				std::cout << "Set book to True\n";
			} else {
				EngineOptions->UseBook = FALSE;
				std::cout << "Set book to False\n";
			}
		} else if (!strncmp(line, "mirror", 6)) {
			int engineSide = BOTH;
			MirrorEvalTest(pos);
			continue;
		} else if (!strncmp(line, "eval", 4)) {
			std::cout << "Eval: " << EvalPosition(pos) << '\n';
		}

		if (info->quit)
			break;
	}
}
