#include "xboard.h"
#include "movegen.h"
#include "makemove.h"
#include "attack.h"
#include "misc.h"
#include "io.h"
#include "evaluate.h"
#include "validate.h"
#include "polybook.h" //for print polykey

#include <iostream>

#define INPUTBUFFER 400 * 6

bool ThreeFoldRep(const S_BOARD *pos) {
    int count = 0;
    for (int i = 0; i < pos->hisPly; ++i) {
        if (pos->history[i].posKey == pos->posKey) {
            count++;
        }
    }
    return (count >= 2);
}

bool DrawMaterial(const S_BOARD *pos) { //check whether one side has enough material to deliver a checkmate
    if (pos->pieceNum[wP] || pos->pieceNum[bP]) return FALSE;
    if (pos->pieceNum[wQ] || pos->pieceNum[wR] 
        || pos->pieceNum[bQ] || pos->pieceNum[bR]) return FALSE;
    if (pos->pieceNum[wB] > 1 || pos->pieceNum[bB] > 1) return FALSE;
    if (pos->pieceNum[wN] > 1 || pos->pieceNum[bN] > 1) return FALSE;
    if (pos->pieceNum[wB] && pos->pieceNum[wN]) return FALSE;
    if (pos->pieceNum[bB] && pos->pieceNum[bN]) return FALSE;
    return TRUE;
}

bool checkResult(S_BOARD *pos) {
    if (pos->fiftyMove > 100) {
        std::cout << "1/2-1/2 {Fifty-move rule}";
        return TRUE;
    }

    if (ThreeFoldRep(pos)) {
        std::cout << "1/2-1/2 {Three-fold rule}\n";
        return TRUE;
    }

    if (DrawMaterial(pos)) {
        std::cout << "1/2-1/2 {Insufficient material}";
        return TRUE;
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    for (int i = 0; i < list->count; ++i) {
        if (!MakeMove(pos, list->moves[i].move)) {
            continue;
        }
        TakeMove(pos); // if there is still legal move -> take move and out.
        return FALSE;
    }

    // if there is no legal move -> check whether it is checkmate or stalemate.
    bool InCheck = SqAttacked(pos->kingSquare[pos->side], pos->side ^ 1, pos);
    if (InCheck) {
        if (pos->side == WHITE) {
            std::cout << "0-1 {Black mates}\n";
            return TRUE;
        } else {
            std::cout << "1-0 {White mates}\n";
            return TRUE;
        }
    } else {
        std::cout << "1/2-1/2 {Stalemate}\n";
        return TRUE;
    }

    return FALSE; //never reached
}
void PrintOptions() {
	std::cout << "feature ping=1 setboard=1 colors=0 usermove=1 memory=1\n";
	std::cout << "feature done=1\n";
}

void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info) {
    setbuf(stdin, NULL);
	setbuf(stdout, NULL);

    int depth = -1, movestogo[2] = {30, 30}, movetime = -1;
	int time = -1, inc = 0;
	int engineSide = BOTH;
	int timeLeft;
	int sec;
	int mps;
	int move = NOMOVE;
	char inBuf[80], command[80];
	int MB;

	engineSide = BLACK;
	ParseFen(START_FEN, pos);
	depth = -1;
	time = -1;

	while (TRUE) {
        if (pos->side == engineSide && checkResult(pos) == FALSE) {
			info->startTime = GetTimeMs();
			info->depth = depth;

			if (time != -1) {
				info->timeSet = TRUE;
				time /= movestogo[pos->side];
				time -= 50;
				info->stopTime = info->startTime + time + inc;
			}

			if (depth == -1 || depth > MAXDEPTH) {
				info->depth = MAXDEPTH;
			}

			std::cout << "time:" << time << " start:" << info->startTime << " stop:" << info->stopTime
                << " depth: " << info->depth << " timeset:" << info->timeSet
                << " movestogo:" << movestogo[pos->side] << " mps:" << mps << '\n';
			
                SearchPosition(pos, info);

			if (mps != 0) {
				movestogo[pos->side^1]--;
				if (movestogo[pos->side ^ 1] < 1) {
					movestogo[pos->side ^ 1] = mps;
				}
			}

		}

		if (!std::cin.getline(inBuf, sizeof(inBuf))) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}

		sscanf(inBuf, "%s", command);

		printf("command seen:%s\n",inBuf);

		if(!strcmp(command, "quit")) {
			info->quit = TRUE;
			break;
		}

		if(!strcmp(command, "force")) {
			engineSide = BOTH;
			continue;
		}

		if(!strcmp(command, "protover")){
			PrintOptions();
		    continue;
		}

		if(!strcmp(command, "sd")) {
			sscanf(inBuf, "sd %d", &depth);
		    std::cout << "DEBUG depth:" << depth << '\n';
			continue;
		}

		if(!strcmp(command, "st")) {
			sscanf(inBuf, "st %d", &movetime);
		    std::cout << "DEBUG movetime:" << movetime << '\n';
			continue;
		}

		if(!strcmp(command, "time")) {
			sscanf(inBuf, "time %d", &time);
			time *= 10;
		    std::cout << "DEBUG time:" << time << '\n';
			continue;
		}

		if(!strcmp(command, "polykey")) {
			PrintBoard(pos);
			std::cout << std::hex << "Polykey:" << PolyKeyFromBoard(pos) << '\n' << std::dec;
			continue;
		}

		if(!strcmp(command, "memory")) {			
			sscanf(inBuf, "memory %d", &MB);		
		    if (MB < 4) MB = 4;
			if (MB > MAX_HASH) MB = MAX_HASH;
			std::cout << "Set Hash to " << MB << " MB\n";
			InitHashTable(pos->HashTable, MB);
			continue;
		}

        if (!strcmp(command, "level")) {
			sec = 0;
			movetime = -1;
			if (sscanf(inBuf, "level %d %d %d", &mps, &timeLeft, &inc) != 3) {
                sscanf(inBuf, "level %d %d:%d %d", &mps, &timeLeft, &sec, &inc);
                std::cout << "DEBUG level with :\n";
			} else {
		        std::cout << "DEBUG level without :\n";
			}
			timeLeft *= 60000;
			timeLeft += sec * 1000;
			movestogo[0] = movestogo[1] = 30;
			if(mps != 0) {
				movestogo[0] = movestogo[1] = mps;
			}
			time = -1;
		    std::cout << "DEBUG level timeLeft:" << timeLeft << " movesToGo:" 
                << movestogo[0] << " inc:" << inc << " mps:" << mps << '\n';
			continue;
		}

        if (!strcmp(command, "ping")) {
			printf("pong%s\n", inBuf+4);
			continue;
		}

		if (!strcmp(command, "new")) {
			engineSide = BLACK;
			ParseFen(START_FEN, pos);
			depth = -1;
			// time = -1;
			continue;
		}

		if (!strcmp(command, "setboard")){
			engineSide = BOTH;
			ParseFen(inBuf + 9, pos);
			continue;
		}

		if (!strcmp(command, "go")) {
			engineSide = pos->side;
			continue;
		}

		if (!strcmp(command, "usermove")){
			movestogo[pos->side]--;
			move = ParseMove(inBuf + 9, pos);
			if (move == NOMOVE) continue;
			MakeMove(pos, move);
            pos->ply = 0;
		}
	}
}

void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info) {
	std::cout << "Welcome to UET In Console Mode!\n";
	std::cout << "Type help for commands\n\n";

	info->GAME_MODE = CONSOLEMODE;
	info->POST_THINKING = TRUE;
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);

	int depth = MAXDEPTH, movetime = 3000;
	int engineSide = BOTH;
	int move = NOMOVE;
	char inBuf[80], command[80];

	engineSide = BLACK;
	ParseFen(START_FEN, pos);

	while (TRUE) {
		if (pos->side == engineSide && checkResult(pos) == FALSE) {
			info->startTime = GetTimeMs();
			info->depth = depth;

			if (movetime != 0) {
				info->timeSet = TRUE;
				info->stopTime = info->startTime + movetime;
			}

			SearchPosition(pos, info);
		}

		std::cout << "\nUET > ";

		if (!std::cin.getline(inBuf, sizeof(inBuf))) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}

		sscanf(inBuf, "%s", command);

		if(!strcmp(command, "help")) {
			std::cout << "Commands:\n";
			std::cout << "quit - quit game\n";
			std::cout << "force - computer will not think\n";
			std::cout << "print - show board\n";
			std::cout << "post - show thinking\n";
			std::cout << "nopost - do not show thinking\n";
			std::cout << "new - start new game\n";
			std::cout << "go - set computer thinking\n";
			std::cout << "depth x - set depth to x\n";
			std::cout << "time x - set thinking time to x seconds (depth still applies if set)\n";
			std::cout << "view - show current depth and movetime settings\n";
			std::cout << "setboard x - set position to fen x\n";
			std::cout << "** note ** - to reset time and depth, set to 0\n";
			std::cout << "enter moves using b7b8q notation\n\n\n";
			continue;
		}

		if(!strcmp(command, "mirror")) {
			engineSide = BOTH;
			MirrorEvalTest(pos);
			continue;
		}

		if(!strcmp(command, "eval")) {
			PrintBoard(pos);
			std::cout << "Eval:" << EvalPosition(pos) << '\n';
			MirrorBoard(pos);
			PrintBoard(pos);
			std::cout << "Eval:" << EvalPosition(pos) << '\n';
			continue;
		}

		if(!strcmp(command, "setboard")){
			engineSide = BOTH;
			ParseFen(inBuf+9, pos);
			continue;
		}

		if(!strcmp(command, "quit")) {
			info->quit = TRUE;
			break;
		}

		if(!strcmp(command, "post")) {
			info->POST_THINKING = TRUE;
			continue;
		}

		if(!strcmp(command, "print")) {
			PrintBoard(pos);
			continue;
		}

		if(!strcmp(command, "nopost")) {
			info->POST_THINKING = FALSE;
			continue;
		}

		if(!strcmp(command, "force")) {
			engineSide = BOTH;
			continue;
		}

		if(!strcmp(command, "view")) {
			if(depth == MAXDEPTH) std::cout << "depth not set ";
			else std::cout << "depth " << depth;

			if(movetime != 0) std::cout << " movetime " << movetime / 1000 << "s\n";
			else std::cout << " movetime not set\n";

			continue;
		}

		if(!strcmp(command, "depth")) {
			sscanf(inBuf, "depth %d", &depth);
		    if(depth==0) depth = MAXDEPTH;
			continue;
		}

		if(!strcmp(command, "time")) {
			sscanf(inBuf, "time %d", &movetime);
			movetime *= 1000;
			continue;
		}

		if(!strcmp(command, "new")) {
			// ClearHashTable(pos->HashTable);
			engineSide = BLACK;
			ParseFen(START_FEN, pos);
			continue;
		}

		if(!strcmp(command, "go")) {
			engineSide = pos->side;
			continue;
		}

		move = ParseMove(inBuf, pos);
		if (move == NOMOVE) {
			std::cout << "Command unknown: " << inBuf << '\n';
			continue;
		}
		MakeMove(pos, move);
		pos->ply = 0;
    }
}