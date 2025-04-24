#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"
#include "init.h"

typedef struct
{

    int starttime;
    int stoptime;
    int depth;
    int timeset;
    int movestogo;

    long nodes;

    int quit;
    int stopped;

    float fh;
    float fhf;
    int nullCut;

    int GAME_MODE;
    int POST_THINKING;

} S_SEARCHINFO;
#endif