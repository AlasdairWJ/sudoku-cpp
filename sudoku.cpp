#include "sudoku.h"

#include <cstdio>
#include <vector>

namespace sudoku
{

namespace
{

using options_t = std::array<std::array<unsigned, 9>, 9>;

void clear_option(options_t& options, int x, int y, int digit)
{
	options[y][x] &= ~(1 << digit);
}

void place_digit(board_t& board, options_t& options, int x, int y, int digit)
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

bool has_option(const options_t& options, int x, int y, int digit)
{
	return (options[y][x] & (1 << digit)) != 0;
}

bool has_single_option(const options_t& options, int x, int y, int& digit)
{
	for (int d = 1; d < 10; d++)
		if (options[y][x] == (1 << d))
		{
			digit = d;
			return true;
		}
	return false;
}

bool is_solved(const board_t& board)
{
	for (int y = 0; y < 9; y++)
		for (int x = 0; x < 9; x++)
			if (board[y][x] == 0)
				return false;
	return true;
}

bool is_solvable(const board_t& board, const options_t& options)
{
	for (int y = 0; y < 9; y++)
		for (int x = 0; x < 9; x++)
			if (board[y][x] == 0 && options[y][x] == 0)
				return false;
	return true;
}

bool do_eliminations(board_t& board, options_t& options)
{
	for (int y = 0; y < 9; y++)
		for (int x = 0; x < 9; x++)
		{
			int digit;
			if (board[y][x] == 0 && has_single_option(options, x, y, digit))
			{
				place_digit(board, options, x, y, digit);
				return true;
			}
		}
	return false;
}

void next_free_pos(board_t& board, int& pos_x, int& pos_y)
{
	for (int y = 0; y < 9; y++)
		for (int x = 0; x < 9; x++)
			if (board[y][x] == 0)
			{
				pos_x = x;
				pos_y = y;
				return;
			}
	puts("bap");
}

}

// ---------------------------------------------------------

bool solve(board_t& the_board)
{
	options_t the_options;

	for (int y = 0; y < 9; y++)
		for (int x = 0; x < 9; x++)
			the_options[y][x] = 0b1111111110;

	for (int y = 0; y < 9; y++)
		for (int x = 0; x < 9; x++)
		{
			const int digit = the_board[y][x];
			if (digit != 0)
				place_digit(the_board, the_options, x, y, digit);
		}

	std::vector<std::pair<board_t, options_t>> current_states;
	current_states.emplace_back(the_board, the_options);

	while (!current_states.empty())
	{
		std::vector<std::pair<board_t, options_t>> next_states;

		for (auto& pair : current_states)
		{
			auto& board = pair.first;
			auto& options = pair.second;

			while (do_eliminations(board, options));

			if (is_solved(board))
			{
				the_board = board;
				return true;
			}

			if (!is_solvable(board, options))
				continue;

			int x, y;
			next_free_pos(board, x, y);
			for (int d = 1; d < 10; d++)
				if (has_option(options, x, y, d))
				{
					auto board_copy = board;
					auto options_copy = options;
					place_digit(board_copy, options_copy, x, y, d);
					next_states.emplace_back(board_copy, options_copy);
				}

		}

		std::swap(current_states, next_states);
	}

	return false;
}

// ---------------------------------------------------------

void print(const board_t& board)
{
	for (int y = 0; y < 9; y++)
	{
		if (y == 3 || y == 6)
			puts("------+-------+------");

		for (int x = 0; x < 9; x++)
		{
			if (x == 3 || x == 6)
				printf("| ");

			const int digit = board[y][x];
			printf("%c ", digit == 0 ? ' ' : digit + '0');
		}
		putchar('\n');
	}
}

}