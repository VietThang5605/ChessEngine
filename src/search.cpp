#include "search.h"

bool IsRepetition(const S_BOARD *pos) {
    for (int i = pos->hisPly - pos->fiftyMove; i < pos->hisPly - 1; ++i) {
		ASSERT(i >= 0 && i < MAXGAMEMOVES);
		if (pos->posKey == pos->history[i].posKey) {
			return TRUE;
		}
	}
	return FALSE;
}