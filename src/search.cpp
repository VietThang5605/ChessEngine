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
#include "types.h"
#include "tinycthread.h"

#include <iomanip>

#define reducedDepth 3

int rootDepth;

thrd_t workerThreads[MAXTHREAD];

static void CheckUp(S_SEARCHINFO *info) { //will be called every 4k node or so
	// .. check if time up, or interrupt from GUI
	if (info->timeSet == TRUE && GetTimeMs() > info->stopTime) {
		info->stopped = TRUE;
	}
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

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table) {
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

	table->overWrite = 0;
	table->hit = 0;
	table->cut = 0;
	table->currentAge++;
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

	if (IsRepetition(pos) || pos->fiftyMove >= 100) {
		return 0;
	}

	if(pos->ply > MAXDEPTH - 1) {
		return EvalPosition(pos);
	}

	int Score = EvalPosition(pos);

	ASSERT(Score > -AB_BOUND && Score < AB_BOUND);

	if (Score >= beta) {
		return beta;
	}

	if (Score > alpha) {
		alpha = Score;
	}

	S_MOVELIST list[1];
    GenerateAllCaptures(pos, list);

	int Legal = 0;
	Score = -AB_BOUND; 

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

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table, int DoNull) {
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

	int Score = -AB_BOUND;
	int PvMove = NOMOVE;

	if (ProbeHashEntry(pos, table, &PvMove, &Score, alpha, beta, depth) == TRUE) {
		table->cut++;
		return Score;
	}

	if (DoNull && !InCheck && pos->ply && (pos->bigPiece[pos->side] > 1) && depth >= reducedDepth) {
		MakeNullMove(pos);
		Score = -AlphaBeta( -beta, -beta + 1, depth - reducedDepth, pos, info, table, FALSE);
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
	int BestScore = -AB_BOUND;
	Score = -AB_BOUND;

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
		Score = -AlphaBeta(-beta, -alpha, depth - 1, pos, info, table, TRUE);
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

					StoreHashEntry(pos, table, BestMove, beta, HFBETA, depth);
					
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
			return -AB_BOUND + pos->ply;
		} else {
			return 0;
		}
	}

	if (alpha != OldAlpha) {
		StoreHashEntry(pos, table, BestMove, BestScore, HFEXACT, depth);
	} else {
		StoreHashEntry(pos, table, BestMove, alpha, HFALPHA, depth);
	}

	return alpha;
}

int SearchPosition_Thread(void *data) {
	S_SEARCH_THREAD_DATA *searchData = (S_SEARCH_THREAD_DATA *)data;
	S_BOARD *pos = new S_BOARD[1];
	memcpy(pos, searchData->originalPosition, sizeof(S_BOARD)); //using memcpy only when data in S_BOARD is trivial types
	SearchPosition(pos, searchData->info, searchData->ttable);
	delete[] pos;
	return 0;
}

void IterativeDeepening(S_SEARCH_WORKER_DATA *workerData) {
	workerData->bestMove = NOMOVE;
	int bestScore = -AB_BOUND;
	int pvMoves = 0;
	int pvNum = 0;
	
    for (int currentDepth = 1; currentDepth <= workerData->info->depth; ++currentDepth ) {
        rootDepth = currentDepth;
        bestScore = AlphaBeta(-AB_BOUND, AB_BOUND, currentDepth, workerData->pos, workerData->info, workerData->ttable, TRUE);

        if (workerData->info->stopped == TRUE) {
            break;
        }
    
		if (workerData->threadNumber == 0) {
            pvMoves = GetPvLine(currentDepth, workerData->pos, workerData->ttable);
            workerData->bestMove = workerData->pos->PvArray[0];

			std::cout << "info score cp " << bestScore << " depth " << currentDepth 
					<< " nodes " << workerData->info->nodes 
					<< " time " << GetTimeMs() - workerData->info->startTime << " ";
			std::cout << "pv";
        
			for (int pvNum = 0; pvNum < pvMoves; ++pvNum) {
				std::cout << " " << PrintMove(workerData->pos->PvArray[pvNum]);
			}
			std::cout << '\n';
        }
    }    
}

int StartWorkerThread(void *data) {
    S_SEARCH_WORKER_DATA *workerData = (S_SEARCH_WORKER_DATA *)data;
	std::cout << "Thread:" << workerData->threadNumber << " Starts\n";
	IterativeDeepening(workerData);
	std::cout << "Thread:" <<  workerData->threadNumber << " Ends; Depth:" << workerData->depth << '\n';
	if (workerData->threadNumber == 0) {
		std::cout << "bestmove " << PrintMove(workerData->bestMove) << '\n';
	}
	delete[] workerData;
	return 0;
}

void SetupWorker(int threadNumber, thrd_t *workerTh, S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table) {
	S_SEARCH_WORKER_DATA *pWorkerData = new S_SEARCH_WORKER_DATA[1];
    pWorkerData->pos = new S_BOARD[1];
	memcpy(pWorkerData->pos, pos, sizeof(S_BOARD));
	pWorkerData->info = info;
	pWorkerData->ttable = table;
	pWorkerData->threadNumber = threadNumber;
	thrd_create(workerTh, &StartWorkerThread, (void*)pWorkerData);
}

void CreateSearchWorkers(S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table) {
	std::cout << "CreateSearchWorkers:"  << info->threadNumber << '\n';
	for (int i = 0; i < info->threadNumber; ++i) {
        SetupWorker(i, &workerThreads[i], pos, info, table);
    }
}

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table) {
	int bestMove = NOMOVE;
	int bestScore = -AB_BOUND;
	int pvMoves = 0;

	ClearForSearch(pos, info, table);

	if (EngineOptions->UseBook == TRUE) {
		bestMove = GetBookMove(pos);

		if (bestMove != NOMOVE) {
			std::cout << "info string book move selected\n";
			std::cout << "info depth 0 pv " << PrintMove(bestMove) << '\n';
			std::cout << "bestmove " << PrintMove(bestMove) << '\n';
			return;
		}
	}

	if (bestMove == NOMOVE) {
		CreateSearchWorkers(pos, info, table);
	}

	for (int i = 0; i < info->threadNumber; ++i) {
		thrd_join(workerThreads[i], NULL);
	}
}