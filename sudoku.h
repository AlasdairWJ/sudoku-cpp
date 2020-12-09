#ifndef __SUDOKU_H
#define __SUDOKU_H

#include <array>

namespace sudoku
{

using board_t = std::array<std::array<int, 9>, 9>;

void print(const board_t& board);
bool solve(board_t& the_board);

}

#endif __SUDOKU_H