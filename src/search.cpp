#include "search.h"
#include "misc.h"
#include "pvtable.h"
#include "io.h"
#include "evaluate.h"
#include "movegen.h"
#include "makemove.h"
#include "attack.h"

#include <iomanip>

static void CheckUp(S_SEARCHINFO *info) { //will be called every 4k node or so
	// .. check if time up, or interrupt from GUI

}

static void PickNextMove(int moveNum, S_MOVELIST *list) {
	int bestScore = 0;
	int bestNum = moveNum;

	for (int i = moveNum; i < list->count; ++i) {
		if (list->moves[i].score > bestScore) {
			bestScore = list->moves[i].score;
			bestNum = i;
		}
	}

	ASSERT(moveNum >= 0 && moveNum < list->count);
	ASSERT(bestNum >= 0 && bestNum < list->count);
	ASSERT(bestNum >= moveNum);

	S_MOVE temp = list->moves[moveNum];
	list->moves[moveNum] = list->moves[bestNum];
	list->moves[bestNum] = temp;
}

bool IsRepetition(const S_BOARD *pos) {
    for (int i = pos->hisPly - pos->fiftyMove; i < pos->hisPly - 1; ++i) {
		ASSERT(i >= 0 && i < MAXGAMEMOVES);
		if (pos->posKey == pos->history[i].posKey) {
			return TRUE;
		}
	}
	return FALSE;
}

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {
	for (int i = 0; i < PIECE_NB; ++i) {
		for (int j = 0; j < BRD_SQ_NUM; ++j) {
			pos->searchHistory[i][j] = 0;
		}
	}

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < MAXDEPTH; ++j) {
			pos->searchKillers[i][j] = 0;
		}
	}

	ClearPvTable(pos->PvTable);
	pos->ply = 0;

	info->startTime = GetTimeMs();
	info->stopped = 0;
	info->nodes = 0;
	info->fh = 0;
	info->fhf = 0;
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
	ASSERT(CheckBoard(pos));
	info->nodes++;

	if ((IsRepetition(pos) || pos->fiftyMove >= 100)&& pos->ply) {
		return 0;
	}

	if(pos->ply > MAXDEPTH - 1) {
		return EvalPosition(pos);
	}

	int Score = EvalPosition(pos);

	if (Score >= beta) {
		return beta;
	}

	if (Score > alpha) {
		alpha = Score;
	}

	S_MOVELIST list[1];
    GenerateAllCaptures(pos, list);

	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	Score = -INFINITE; 
	// int PvMove = ProbePvTable(pos);

	for (int MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		PickNextMove(MoveNum, list);

		if (!MakeMove(pos, list->moves[MoveNum].move)) {
			continue;
		}

		Legal++;
		Score = -Quiescence(-beta, -alpha, pos, info);
		TakeMove(pos);

		if (Score > alpha) {
			if (Score >= beta) {
				if (Legal == 1) {
					info->fhf++;
				}
				info->fh++;
				
				return beta;
			}
			alpha = Score;
			BestMove = list->moves[MoveNum].move;
		}
	}

	if (alpha != OldAlpha) {
		StorePvMove(pos, BestMove);
	}
	return alpha;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull) {
	ASSERT(CheckBoard(pos));

	if (depth == 0) {
		return Quiescence(alpha, beta, pos, info);
	}

	info->nodes++;

	if (IsRepetition(pos) || pos->fiftyMove >= 100) {
		return 0;
	}

	if (pos->ply > MAXDEPTH - 1) {
		return EvalPosition(pos);
	}

	S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	int Score = -INFINITE; 
	int PvMove = ProbePvTable(pos);

	if (PvMove != NOMOVE) {
		for (int MoveNum = 0; MoveNum < list->count; MoveNum++) {
			if (list->moves[MoveNum].move == PvMove) {
				list->moves[MoveNum].score = 2000000;
				break;
			}
		}
	}

	for (int MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		PickNextMove(MoveNum, list);

		if (!MakeMove(pos, list->moves[MoveNum].move)) {
			continue;
		}

		Legal++;
		Score = -AlphaBeta(-beta, -alpha, depth - 1, pos, info, TRUE);
		TakeMove(pos);

		if (Score > alpha) {
			if (Score >= beta) {
				if (Legal == 1) {
					info->fhf++;
				}
				info->fh++;

				if (!(list->moves[MoveNum].move & MOVEFLAG_CAPTURED)) {
					pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
					pos->searchKillers[0][pos->ply] = list->moves[MoveNum].move;
				}
				
				return beta;
			}
			alpha = Score;
			BestMove = list->moves[MoveNum].move;
			if (!(list->moves[MoveNum].move & MOVEFLAG_CAPTURED)) {
				pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][TOSQ(BestMove)] += depth;
			}
		}
	}

	if (Legal == 0) {
		if (SqAttacked(pos->kingSquare[pos->side], pos->side ^ 1, pos)) {
			return -MATE + pos->ply;
		} else {
			return 0;
		}
	}

	if (alpha != OldAlpha) {
		StorePvMove(pos, BestMove);
	}

	return alpha;
}

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
	int bestMove = NOMOVE;
	int bestScore = -INFINITE;
	int pvMoves = 0;

	ClearForSearch(pos, info);

	for (int currentDepth = 1; currentDepth <= info->depth; ++currentDepth) {
		bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, pos, info, TRUE);

		if (info->stopped == TRUE) {
			break;
		}

		pvMoves = GetPvLine(currentDepth, pos);
		bestMove = pos->PvArray[0];
		std::cout << "Depth: " << currentDepth << " score: " << bestScore << " move: " 
				<< PrintMove(bestMove) << " nodes: " << info->nodes << '\n';

		std::cout << "PvLine of " << pvMoves << " Moves: ";
		for (int pvNum = 0; pvNum < pvMoves; ++pvNum) {
			std::cout << " " << PrintMove(pos->PvArray[pvNum]);
		}
		std::cout << '\n';
		std::cout << "Ordering: " << std::fixed << std::setprecision(2) << (info->fhf / info->fh) << '\n';
	}
}