#include <cstdio>
#include <vector>
#include <algorithm>

// ---------------------------------------------------------

#define DEFAULT_SUDOKU_FILENAME "sudoku.txt"

// ---------------------------------------------------------

struct sudoku_t
{
	struct cell_t
	{
		cell_t() : digit(0), num_options(9)
		{
			for (bool& opt : this->options)
				opt = true;
		}

		void set(int n)
		{	
			this->digit = n;
			this->num_options = 1;
			for (int d = 1; d < 10; d++)
				options[d] = d == n;
		}

		bool clear(int n)
		{
			if (this->digit == 0 && this->options[n])
			{
				this->options[n] = false;
				return --this->num_options == 1;
			}
			return false;
		}

		int next_option() const
		{
			for (int d = 1; d < 10; d++)
				if (this->options[d])
					return d;

			return 0;
		}

		bool is_valid() const
		{
			return digit != 0 || num_options != 0;
		}

		int digit, num_options;
		bool options[10];
	};

	sudoku_t() = default;
	sudoku_t(const sudoku_t& other) = default;
	sudoku_t(const int (&the_grid)[9][9])
	{
		for (int y = 0; y < 9; y++)
			for (int x = 0; x < 9; x++)
				if (the_grid[y][x] != 0)
					this->set(x, y, the_grid[y][x]);
	}

	bool is_solved() const
	{
		for (const auto& row : this->grid)
			for (const auto& cell : row)
				if (cell.digit == 0)
					return false;

		return true;
	}

	bool is_valid() const
	{
		for (const auto& row : this->grid)
			for (const auto& cell : row)
				if (!cell.is_valid())
					return false;

		return true;
	}

	bool set(int x, int y, int digit)
	{
		this->grid[x][y].set(digit);

		bool any = false;

		const int bx = x / 3 * 3, by = y / 3 * 3;
		for (int z = 0; z < 9; z++)
		{
			any |= this->grid[x][z].clear(digit);
			any |= this->grid[z][y].clear(digit);
			any |= this->grid[bx + z/3][by + z%3].clear(digit);
		}

		return any;
	}

	bool try_elim()
	{
		bool any = false;
		for (int x = 0; x < 9; x++)
			for (int y = 0; y < 9; y++)
				if (grid[x][y].digit == 0 && grid[x][y].num_options == 1)
				{
					int digit = grid[x][y].next_option();
					any |= this->set(x, y, digit);
					// printf("put %d at %d, %d\n", digit, x, y);
				}

		return any;
	}

	int at(int x, int y) const
	{
		return this->grid[x][y].digit;
	}

	bool next_free_pos(int& pos_x, int& pos_y)
	{
		for (int x = 0; x < 9; x++)
			for (int y = 0; y < 9; y++)
				if (this->grid[x][y].digit == 0)
				{
					pos_x = x;
					pos_y = y;
					return true;
				}

		return false;
	}

	cell_t grid[9][9];
};

// ---------------------------------------------------------

void print(const sudoku_t& sudoku)
{
	for (int y = 0; y < 9; y++)
	{
		if (y == 3 || y == 6)
			puts("------+-------+------");

		for (int x = 0; x < 9; x++)
		{
			if (x == 3 || x == 6)
				printf("| ");

			int digit = sudoku.at(x, y);
			printf("%c ", digit == 0 ? ' ' : digit + '0');
		}
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------------------------------------

bool solve(sudoku_t& target_sudoku)
{
	std::vector<sudoku_t> current_states, next_states;
	current_states.push_back(target_sudoku);

	while (current_states.size() != 0)
	{
		for (sudoku_t& sudoku : current_states)
		{
			while (sudoku.try_elim());
			
			if (sudoku.is_solved())
			{
				target_sudoku = sudoku;
				return true;
			}
			
			if (!sudoku.is_valid())
				continue;

			int x, y;
			sudoku.next_free_pos(x, y);
			for (int d = 1; d < 10; d++)
			{
				if (sudoku.grid[x][y].options[d])
				{
					sudoku_t copy = sudoku;
					copy.set(x, y, d);
					next_states.push_back(copy);
				}
			}
		}

		std::swap(current_states, next_states);
		next_states.clear();
	}

	return false;
}

// ---------------------------------------------------------

bool load_from_file(const char* filename, sudoku_t& sudoku)
{
	FILE* file = fopen(filename, "r");

	if (file == nullptr)
		return false;

	for (int y = 0; y < 9; y++)
	{
		char row[10];
		fscanf_s(file, "%s\n", row, 10);

		for (int x = 0; x < 9; x++)
			if (row[x] != '0')
				sudoku.set(x, y, row[x] - '0');
	}

	fclose(file);

	return true;
}

// ---------------------------------------------------------

int main(int argc, const char* argv[])
{
	const char* sudoku_filename = argc > 1 ? argv[1] : DEFAULT_SUDOKU_FILENAME;
	
	sudoku_t sudoku;
	if (!load_from_file(sudoku_filename, sudoku))
	{
		printf("failed to load file \"%s\"", sudoku_filename);
		return 1;
	}

	if (solve(sudoku))
	{
		print(sudoku);
	}
	else
	{
		puts("impossible!");
	}

	return 0;
}