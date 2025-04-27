#include "uci.h"
#include "io.h"
#include "makemove.h"

#include <iostream>
#include <cstdio>
#include <cstring>

#define INPUTBUFFER 400 * 6

// void ParseGo(char *line, S_SEARCHINFO *info, S_BOARD *pos)
// {
//     int depth = -1, movestogo = 30, movetime = -1;
//     int time = -1, inc = 0;
//     char *ptr = nullptr;
//     info->timeset = FALSE;

//     if ((ptr = strstr(line, "infinite")))
//     {
//         // infinite search, do nothing
//     }

//     if ((ptr = strstr(line, "binc")) && pos->side == BLACK)
//     {
//         inc = std::atoi(ptr + 5);
//     }

//     if ((ptr = strstr(line, "winc")) && pos->side == WHITE)
//     {
//         inc = std::atoi(ptr + 5);
//     }

//     if ((ptr = strstr(line, "wtime")) && pos->side == WHITE)
//     {
//         time = std::atoi(ptr + 6);
//     }

//     if ((ptr = strstr(line, "btime")) && pos->side == BLACK)
//     {
//         time = std::atoi(ptr + 6);
//     }

//     if ((ptr = strstr(line, "movestogo")))
//     {
//         movestogo = std::atoi(ptr + 10);
//     }

//     if ((ptr = strstr(line, "movetime")))
//     {
//         movetime = std::atoi(ptr + 9);
//     }

//     if ((ptr = strstr(line, "depth")))
//     {
//         depth = std::atoi(ptr + 6);
//     }

//     if (movetime != -1)
//     {
//         time = movetime;
//         movestogo = 1;
//     }

//     info->starttime = GetTimeMs();
//     info->depth = depth;

//     if (time != -1)
//     {
//         info->timeset = TRUE;
//         time /= movestogo;
//         time -= 50;
//         info->stoptime = info->starttime + time + inc;
//     }

//     if (depth == -1)
//     {
//         info->depth = MAXDEPTH;
//     }

//     std::printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
//                 time, info->starttime, info->stoptime, info->depth, info->timeset);

//     SearchPosition(pos, info);
// }

void ParsePosition(char *lineIn, S_BOARD *pos)
{
    lineIn += 9;
    char *ptrChar = lineIn;

    if (strncmp(lineIn, "startpos", 8) == 0)
    {
        ParseFen(const_cast<char *>(START_FEN), pos);
    }
    else
    {
        ptrChar = strstr(lineIn, "fen");
        if (ptrChar == nullptr)
        {
            ParseFen(const_cast<char *>(START_FEN), pos);
        }
        else
        {
            ptrChar += 4;
            ParseFen(ptrChar, pos);
        }
    }

    ptrChar = strstr(lineIn, "moves");
    int move;

    if (ptrChar != nullptr)
    {
        ptrChar += 6;
        while (*ptrChar)
        {
            move = ParseMove(ptrChar, pos);
            if (move == NOMOVE)
                break;
            MakeMove(pos, move);
            pos->ply = 0;
            while (*ptrChar && *ptrChar != ' ')
                ptrChar++;
            ptrChar++;
        }
    }
    PrintBoard(pos);
}

void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info)
{
    info->GAME_MODE = UCIMODE;

    std::setbuf(stdin, nullptr);
    std::setbuf(stdout, nullptr);

    char line[INPUTBUFFER];
    std::printf("id name %s\n", NAME);
    std::printf("id author Bluefever\n");
    std::printf("option name Hash type spin default 64 min 4 max %d\n", MAX_HASH);
    std::printf("option name Book type check default true\n");
    std::printf("uciok\n");

    int MB = 64;

    while (TRUE)
    {
        std::memset(&line[0], 0, sizeof(line));
        std::fflush(stdout);
        if (!std::fgets(line, INPUTBUFFER, stdin))
            continue;
        if (line[0] == '\n')
            continue;

        if (!std::strncmp(line, "isready", 7))
        {
            std::printf("readyok\n");
            continue;
        }
        else if (!std::strncmp(line, "position", 8))
        {
            ParsePosition(line, pos);
        }
        else if (!std::strncmp(line, "ucinewgame", 10))
        {
            char startpos[] = "position startpos\n";
            ParsePosition(startpos, pos);
        }
        // else if (!std::strncmp(line, "go", 2))
        // {
        //     std::printf("Seen Go..\n");
        //     ParseGo(line, info, pos);
        // }
        else if (!std::strncmp(line, "quit", 4))
        {
            info->quit = TRUE;
            break;
        }
        else if (!std::strncmp(line, "uci", 3))
        {
            std::printf("id name %s\n", NAME);
            std::printf("id author Bluefever\n");
            std::printf("uciok\n");
        }
        // else if (!std::strncmp(line, "debug", 5))
        // {
        //     DebugAnalysisTest(pos, info);
        //     break;
        // }
        // else if (!std::strncmp(line, "setoption name Hash value ", 26))
        // {
        //     std::sscanf(line, "%*s %*s %*s %*s %d", &MB);
        //     if (MB < 4)
        //         MB = 4;
        //     if (MB > MAX_HASH)
        //         MB = MAX_HASH;
        //     std::printf("Set Hash to %d MB\n", MB);
        //     InitHashTable(pos->HashTable, MB);
        // }
        // else if (!std::strncmp(line, "setoption name Book value ", 26))
        // {
        //     char *ptrTrue = strstr(line, "true");
        //     if (ptrTrue != nullptr)
        //     {
        //         EngineOptions->UseBook = TRUE;
        //     }
        //     else
        //     {
        //         EngineOptions->UseBook = FALSE;
        //     }
        // }

        if (info->quit)
            break;
    }
}
