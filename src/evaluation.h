#ifndef EVALUATION_H_INCLUDED
#define EVALUATION_H_INCLUDED

#include <string>
#include "board.h" // Cần định nghĩa S_Board
#include "evaluation_types.h"

class Position;

namespace Eval {

    constexpr SF::Value Tempo = SF::Value(28); // Must be visible to search

    std::string trace(const Position& pos);

    SF::Value evaluate(const Position& pos);
}



#endif // EVALUATION_H_INCLUDED