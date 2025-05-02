#include "search.h"
#include "misc.h"
#include "pvtable.h"
#include "io.h"
#include "evaluate.h"
#include "movegen.h"
#include "makemove.h"
#include "attack.h"
#include "ucioption.h"
#include "polybook.h"

#include <iomanip>

#define reducedDepth 3

static void CheckUp(S_SEARCHINFO *info) { //will be called every 4k node or so
	// .. check if time up, or interrupt from GUI
	if (info->timeSet == TRUE && GetTimeMs() > info->stopTime) {
		info->stopped = TRUE;
	}

	ReadInput(info);
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

	pos->HashTable->overWrite=0;
	pos->HashTable->hit=0;
	pos->HashTable->cut=0;
	pos->ply = 0;

	info->stopped = 0;
	info->nodes = 0;
	info->fh = 0;
	info->fhf = 0;
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
	ASSERT(CheckBoard(pos));
	ASSERT(beta>alpha);

	if ((info->nodes & 2047) == 0) {
		CheckUp(info);
	}

	info->nodes++;

	if ((IsRepetition(pos) || pos->fiftyMove >= 100) && pos->ply) {
		return 0;
	}

	if(pos->ply > MAXDEPTH - 1) {
		return EvalPosition(pos);
	}

	int Score = EvalPosition(pos);

	ASSERT(Score > -INF && Score < INF);

	if (Score >= beta) {
		return beta;
	}

	if (Score > alpha) {
		alpha = Score;
	}

	S_MOVELIST list[1];
    GenerateAllCaptures(pos, list);

	int Legal = 0;
	Score = -INF; 

	for (int MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		PickNextMove(MoveNum, list);

		if (!MakeMove(pos, list->moves[MoveNum].move)) {
			continue;
		}

		Legal++;
		Score = -Quiescence(-beta, -alpha, pos, info);
		TakeMove(pos);

		if (info->stopped == TRUE) {
			return 0;
		}

		if (Score > alpha) {
			if (Score >= beta) {
				if (Legal == 1) {
					info->fhf++;
				}
				info->fh++;
				
				return beta;
			}
			alpha = Score;
		}
	}

	return alpha;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull) {
	ASSERT(CheckBoard(pos));
	ASSERT(depth >= 0);

	if (depth == 0) {
		return Quiescence(alpha, beta, pos, info);
	}

	if ((info->nodes & 2047) == 0) {
		CheckUp(info);
	}

	info->nodes++;

	if ((IsRepetition(pos) || pos->fiftyMove >= 100) && pos->ply) { //its a draw
		return 0;
	}

	if (pos->ply > MAXDEPTH - 1) {
		return EvalPosition(pos);
	}

	bool InCheck = SqAttacked(pos->kingSquare[pos->side], pos->side ^ 1, pos);

	if (InCheck == TRUE) {
		depth++;
	}

	int Score = -INF;
	int PvMove = NOMOVE;

	if (ProbeHashEntry(pos, &PvMove, &Score, alpha, beta, depth) == TRUE) {
		pos->HashTable->cut++;
		return Score;
	}

	if (DoNull && !InCheck && pos->ply && depth >= reducedDepth) {
		MakeNullMove(pos);
		Score = -AlphaBeta( -beta, -beta + 1, depth - reducedDepth, pos, info, FALSE);
		TakeNullMove(pos);

		if (info->stopped == TRUE) {
			return 0;
		}

		if (Score >= beta && abs(Score) < ISMATE) {
			info->nullCut++;
			return beta;
		}
	}

	S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	int BestScore = -INF;
	Score = -INF;

	if (PvMove != NOMOVE) {
		for (int MoveNum = 0; MoveNum < list->count; ++MoveNum) {
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

		if (info->stopped == TRUE) {
			return 0;
		}

		if (Score > BestScore) {
			BestScore = Score;
			BestMove = list->moves[MoveNum].move;
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

					StoreHashEntry(pos, BestMove, beta, HFBETA, depth);
					
					return beta;
				}
				alpha = Score;

				if (!(list->moves[MoveNum].move & MOVEFLAG_CAPTURED)) {
					pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][TOSQ(BestMove)] += depth;
				}
			}
		}
	}

	if (Legal == 0) {
		if (InCheck) {
			return -ISMATE + pos->ply;
		} else {
			return 0;
		}
	}

	if (alpha != OldAlpha) {
		StoreHashEntry(pos, BestMove, BestScore, HFEXACT, depth);
	} else {
		StoreHashEntry(pos, BestMove, alpha, HFALPHA, depth);
	}

	return alpha;
}

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
	int bestMove = NOMOVE;
	int bestScore = -INF;
	int pvMoves = 0;

	ClearForSearch(pos, info);

	if (EngineOptions->UseBook == TRUE) {
		bestMove = GetBookMove(pos);

		if (bestMove != NOMOVE && info->GAME_MODE == UCIMODE) {
			std::cout << "info string book move selected\n";
			std::cout << "info depth 0 pv " << PrintMove(bestMove) << '\n';
			return;
		}
	}

	if (bestMove == NOMOVE) {
		for (int currentDepth = 1; currentDepth <= info->depth; ++currentDepth) {
			bestScore = AlphaBeta(-INF, INF, currentDepth, pos, info, TRUE);

			if (info->stopped == TRUE) {
				break;
			}

			pvMoves = GetPvLine(currentDepth, pos);
			bestMove = pos->PvArray[0];
			
			if (info->GAME_MODE == UCIMODE) {
				std::cout << "info score cp " << bestScore << " depth " << currentDepth << " nodes " << info->nodes << " time " << GetTimeMs() - info->startTime << " ";
			} else if(info->GAME_MODE == XBOARDMODE && info->POST_THINKING == TRUE) {
				std::cout << currentDepth << ' ' << bestScore << ' ' 
					<< (GetTimeMs() - info->startTime) / 10 << ' ' << info->nodes << ' ';
			} else if(info->POST_THINKING == TRUE) {
				std::cout << "score:" << bestScore << " depth:" << currentDepth
					<< " nodes:" << info->nodes << " time:" << GetTimeMs() - info->startTime << "(ms) ";
			}

			if (info->GAME_MODE == UCIMODE || info->POST_THINKING == TRUE) {
				if (!(info->GAME_MODE == XBOARDMODE)) {
					std::cout << "pv";
				}

				for (int pvNum = 0; pvNum < pvMoves; ++pvNum) {
					std::cout << " " << PrintMove(pos->PvArray[pvNum]);
				}
				std::cout << '\n';
				// std::cout << "Ordering: " << std::fixed << std::setprecision(2) << (info->fhf / info->fh) << '\n';
			}
		}
	}

	if (info->GAME_MODE == UCIMODE) {
		std::cout << "bestmove " << PrintMove(bestMove) << '\n';
	} else if(info->GAME_MODE == XBOARDMODE) {
		std::cout << "move " << PrintMove(bestMove) << '\n';
		MakeMove(pos, bestMove);	
	} else {
		std::cout << "\n\n*** Engine makes move " << PrintMove(bestMove) << " ***\n\n";
		MakeMove(pos, bestMove);
		PrintBoard(pos);
	}
}