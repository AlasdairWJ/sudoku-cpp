#include "sudoku.h"

#include <vector>
#include <string>

namespace sudoku
{

namespace
{

using Options = std::array<std::array<unsigned, 9>, 9>;

void clear_option(Options& options, const int x, const int y, const int digit)
{
	options[y][x] &= ~(1u << digit);
}

void place_digit(Board& board, Options& options, const int x, const int y, const int digit)
{
	board[y][x] = digit;

	const int bx = x / 3 * 3, by = y / 3 * 3;
	for (int z = 0; z < 9; z++)
	{
		clear_option(options, x, z, digit);
		clear_option(options, z, y, digit);
		clear_option(options, bx + z/3, by + z%3, digit);
	}
}

bool has_option(const Options& options, const int x, const int y, const int digit)
{
	return (options[y][x] & (1 << digit)) != 0;
}

bool has_single_option(const Options& options, const int x, const int y, int& digit)
{
	for (digit = 1; digit < 10; digit++)
		if (options[y][x] == (1 << digit))
			return true;
	return false;
}


bool is_solvable(const Board& board, const Options& options)
{
	for (int y = 0; y < 9; y++)
		for (int x = 0; x < 9; x++)
			if (board[y][x] == 0 && options[y][x] == 0)
				return false;
	return true;
}

void do_eliminations(Board& board, Options& options)
{
	bool any_changes;
	do
	{
		any_changes = false;
		for (int y = 0; y < 9; y++)
			for (int x = 0; x < 9; x++)
				if (int digit; board[y][x] == 0 && has_single_option(options, x, y, digit))
				{
					place_digit(board, options, x, y, digit);
					any_changes = true;
				}
	}
	while (any_changes);
}

bool try_get_next_free_pos(Board& board, int& x, int& y)
{
	for (y = 0; y < 9; y++)
		for (x = 0; x < 9; x++)
			if (board[y][x] == 0)
				return true;
	return false;
}

}

// ---------------------------------------------------------

bool solve(Board& the_board)
{
	Options the_options;

	for (int y = 0; y < 9; y++)
		for (int x = 0; x < 9; x++)
			the_options[y][x] = 0b1111111110;

	for (int y = 0; y < 9; y++)
		for (int x = 0; x < 9; x++)
			if (const int digit = the_board[y][x]; digit != 0)
				place_digit(the_board, the_options, x, y, digit);

	std::vector<std::pair<Board, Options>> current_states;
	current_states.emplace_back(the_board, the_options);

	while (!current_states.empty())
	{
		std::vector<std::pair<Board, Options>> next_states;

		for (auto& [board, options] : current_states)
		{
			do_eliminations(board, options);	

			if (!is_solvable(board, options))
				continue;

			if (int x, y; try_get_next_free_pos(board, x, y))
			{
				for (int d = 1; d < 10; d++)
					if (has_option(options, x, y, d))
					{
						auto board_copy = board;
						auto options_copy = options;
						place_digit(board_copy, options_copy, x, y, d);
						next_states.emplace_back(board_copy, options_copy);
					}
			}
			else
			{
				the_board = board;
				return true;
			}
		}

		current_states.swap(next_states);
	}

	return false;
}

}

// ---------------------------------------------------------

std::istream& operator>>(std::istream& is, sudoku::Board& board)
{
	std::string line;
	for (int y = 0; y < 9; y++)
	{
		std::getline(is, line);

		if (y == 3 || y == 6)
			std::getline(is, line);

		for (int x = 0; x < 9; x++)
		{
			const char digit = line[x + x/3];
			board[y][x] = (digit == ' ' ? 0 : static_cast<int>(digit - '0'));
		}
	}
	return is;
}

std::ostream& operator<<(std::ostream& os, const sudoku::Board& board)
{
	for (int y = 0; y < 9; y++)
	{
		if (y == 3 || y == 6)
			os << "---+---+---" << std::endl;

		for (int x = 0; x < 9; x++)
		{
			if (x == 3 || x == 6)
				os.put('|');

			const int digit = board[y][x];
			os.put(digit == 0 ? ' ' : digit + '0');
		}
		
		if (y != 8)
			os << std::endl;
	}
	return os;
}