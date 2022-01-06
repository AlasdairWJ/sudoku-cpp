#ifndef __SUDOKU_H
#define __SUDOKU_H

#include <array>
#include <iostream>

namespace sudoku
{

using Board = std::array<std::array<int, 9>, 9>;

bool solve(Board& the_board);

}

std::istream& operator>>(std::istream& is, sudoku::Board& board);
std::ostream& operator<<(std::ostream& os, const sudoku::Board& board);

#endif __SUDOKU_H