#include "Maze.h"


const unsigned int SIGNED_MASK = 0x7FFFFFF;

void Maze::SetPlayerPos(char PlayerChar, int NewX_Pos, int NewY_Pos, int Current_X, int Current_Y)
{
	p_Cells[Current_Y][Current_X].display = ' ';
	p_Cells[NewY_Pos][NewX_Pos].display = PlayerChar;
}

void Maze::ClearCheeze(char PlayerChar, int CheezeY, int CheezeX, int NewX_Pos, int NewY_Pos, int Current_X, int Current_Y)
{
	p_Cells[Current_Y][Current_X].display = ' ';
	p_Cells[CheezeY][CheezeX].display = ' ';
	p_Cells[NewY_Pos][NewX_Pos].display = PlayerChar;
}

//void Maze::ResetPlayerPos(char PlayerChar, int NewX_Pos, int NewY_Pos, int CurrentX, int CurrentY)
//{
//	p_Cells[CurrentX][CurrentY].display = ' ';
//	p_Cells[NewY_Pos][NewX_Pos].display = PlayerChar;
//
//}

void Maze::SetEnemyPos(char SnakeChar, int NewPosX, int NewPosY, int CurrentX, int CurrentY)
{
	p_Cells[CurrentY][CurrentX].display = ' ';
	p_Cells[NewPosY][NewPosX].display = SnakeChar;
}

bool Maze::UpdateMaze()
{

	for (int r = 0; r < m_dimensions.y; r++)
	{
		for (int c = 0; c < m_dimensions.x; c++)
		{
			p_chars[r][c] = p_Cells[r][c].display;
			
		}
		std::string s(p_chars[r], m_dimensions.x);
		m_strings.push_back(s);
	}
	
	/*char str[4] = {};*/
	
	return false;
}

bool Maze::CheckWallChar(int Wall_X, int Wall_Y)
{
	return p_Cells[Wall_Y][Wall_X].display == getWallChar();
}

bool Maze::CheckFloorChar(int Wall_X, int Wall_Y)
{
	return p_Cells[Wall_Y][Wall_X].display == getFloorChar();
}

//-----------------------------------------------------------------------------
Maze::Maze()
{
	p_Cells = nullptr;
	p_chars = nullptr;
}

//-----------------------------------------------------------------------------
Maze::~Maze()
{
	reset();
}

double	Maze::calcTimeLimit_ms()
{
	double limit = sqrt(m_dimensions.x * m_dimensions.y) / 10 * 1000;
	return limit;
}

void Maze::reset()
{
	if (p_Cells != nullptr)
	{
		delete[] p_Cells[0];
		delete[] p_Cells;
		p_Cells = nullptr;
	}
	if (p_chars != nullptr)
	{
		delete[] p_chars[0];
		delete[] p_chars;
		p_chars = nullptr;
	}

	for (auto stringItr = m_strings.begin(); stringItr != m_strings.end(); stringItr++)
		stringItr->erase();
	m_strings.erase(m_strings.begin(), m_strings.end());
	m_endpoints.erase(m_endpoints.begin(), m_endpoints.end());

	m_msLimit = 0;
	m_start.x = m_start.y = m_exit.x = m_exit.y = m_dimensions.x = m_dimensions.y = 0;
}

bool    Maze::create(CheezePoints* Cheezes, SnakeEnemy* Snakes, const int nCheeze,const int nSnakes, int seed, int width, int height, char PlayerSimbol, int X, int Y)
{
	// validate input: dimensions must be odd numbers. Shrink by 1 if even
	if (MINIMUM_SIZE >= width || MINIMUM_SIZE >= height || MAXIMUM_WIDTH < width || MAXIMUM_HEIGHT < height)
		return false;

	// if re-creating the maze, set key members back to clean starting point
	if (nullptr != p_chars)
		reset();

	// store and apply the random seed: the one passed in, or generate one
	if (NO_SEED == seed)
	{
		seed = (int)(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	}

	m_seed = seed;
	m_mtRand.seed(m_seed);

	// store the values passed in - might need to change
	m_RequestedDimensions.x = width;
	m_RequestedDimensions.y = height;

	// adjust the width and height to force odd size
	if (0 == (width & 0x1))		// the low bit is set if an odd number
		width--;
	if (0 == (height & 0x1))	// the low bit is set if an odd number
		height--;

	m_dimensions.x = width;
	m_dimensions.y = height;

	// Create the temporary data structures and generate the actual maze
	initialize();
	generateMaze(Cheezes,Snakes,nCheeze ,nSnakes, PlayerSimbol, X, Y);

	if (m_printDuringGeneration)
		printCells();

	// now create a 2D character array; don't always need/want the Cells after generating the maze
	p_chars = new char* [height];
	*p_chars = new char[width * height];
	for (int r = 1; r < height; r++)
	{
		p_chars[r] = p_chars[r - 1];
	}


	// and copy the display characters from the Cell array to the char array and the vector of strings
	for (int r = 0; r < m_dimensions.y; r++)
	{
		for (int c = 0; c < m_dimensions.x; c++)
			p_chars[r][c] = p_Cells[r][c].display;
		std::string s(p_chars[r], m_dimensions.x);
		char str[4] = {};
		m_strings.push_back(s);
	}

	// set the traversal target for the new maze
	m_msLimit = calcTimeLimit_ms();

	return true;
}

void Maze::printCells()
{
#if (_ENABLE_STEP_PRINTING)
	COORD position;
	position.X = 0;
	position.Y = 0;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);

	for (int r = 0; r < m_dimensions.y; r++)
	{
		std::cout << std::endl;
		for (int c = 0; c < m_dimensions.x; c++)
			std::cout << p_Cells[r][c].display;
	}
#endif
}

//-----------------------------------------------------------------------------
bool Maze::createRandomSize(CheezePoints* Cheezes,SnakeEnemy* Snakes, const int nCheeze,const int nSnakes, int seed, char PlayerSimbol, int X, int Y)
{
	// store and apply the random seed: the one passed in, or generate one
	if (seed == NO_SEED)
	{
		seed = (int)std::chrono::high_resolution_clock::now().time_since_epoch().count();
	}

	std::mt19937 mtRand(seed);

	int width = (mtRand() % (MAXIMUM_WIDTH - MINIMUM_SIZE - 1) + MINIMUM_SIZE) | 1;
	int height = (mtRand() % (MAXIMUM_HEIGHT - MINIMUM_SIZE - 1) + MINIMUM_SIZE) | 1;
	bool created = create(Cheezes, Snakes, nCheeze,nSnakes, seed, width, height, PlayerSimbol, X, Y);
	assert(created);
	assert(p_chars);
	return true;
}

//-----------------------------------------------------------------------------
void Maze::initialize()
{
	// Allocate a 2D matrix of Cells, using a sneaky approach that allows use of [][] notation.
	//  This code actually creates an array of Cell pointers, and then a block of Cells.
	//  The array of pointers is then initialized to point into the block, so each pointer 
	//  accesses a "row" from 0 to Height.
	p_Cells = new Cell* [m_dimensions.y];
	*p_Cells = new Cell[m_dimensions.x * m_dimensions.y];
	for (int r = 1; r < m_dimensions.y; r++)
		p_Cells[r] = p_Cells[r - 1] + m_dimensions.x;

	for (int r = 0; r < m_dimensions.y; r++)
	{
		for (int c = 0; c < m_dimensions.x; c++)
		{
			p_Cells[r][c].display = m_wallChar;
			p_Cells[r][c].visited = false;
			p_Cells[r][c].top_wall = true;
			p_Cells[r][c].bot_wall = true;
			p_Cells[r][c].left_wall = true;
			p_Cells[r][c].right_wall = true;
		}
	}
	for (int r = 1; r < m_dimensions.y - 1; r++)
	{
		for (int c = 1; c < m_dimensions.x - 1; c++)
		{
			// Border Cells have fewer accessible walls
			p_Cells[1][c].top_wall = false;
			p_Cells[m_dimensions.y - 2][c].bot_wall = false;
			p_Cells[r][1].left_wall = false;
			p_Cells[r][m_dimensions.x - 2].right_wall = false;
		}
	}
}

//-----------------------------------------------------------------------------
void Maze::generateMaze(CheezePoints* Cheezes,SnakeEnemy* Snakes, const int nCheeze,const int nSnakes, char PlayerSimbol, int X, int Y)
{
	// Generate a random odd position between 1 and width - 2, and 1 and height - 1.
	// Use SIGNED_MASK to force the random number to be positive when converted from unsigned int to signed int.
	Vector randPos(SIGNED_MASK & (2 * m_mtRand() + 1) % (m_dimensions.x - 1), SIGNED_MASK & (2 * m_mtRand() + 1) % (m_dimensions.y - 1));

	// Save this position as the player starting point
	m_start = randPos;
	m_endpoints.push_back(randPos);

	// counter for all the cells visited while generating the maze. done when all have been visited.
	int visitedCells = 1;
	int totalCells = ((m_dimensions.x - 1) / 2) * ((m_dimensions.y - 1) / 2);

	// Stack is used to trace the reverse path
	std::stack<int> back_trackX, back_trackY;
	std::stack<Vector> back_track;

	// Set the start cell
	p_Cells[randPos.y][randPos.x].display = getStartChar();
	// Set start cell as visited;
	p_Cells[randPos.y][randPos.x].visited = true;

	bool just_moved = false;
	char endpointChar = 'a';

	// Now - randomly wander through the cells adjacent to the current cell, until all cells are visited. 
	//  This guarantees there is a path from the starting cell to a finishing cell.
	while (visitedCells < totalCells)
	{
		// test to make sure the adjacent cells are valid
		if ((randPos.y >= 2 && (p_Cells[randPos.y - 2][randPos.x].visited == false) && (p_Cells[randPos.y][randPos.x].top_wall == true && p_Cells[randPos.y - 2][randPos.x].bot_wall == true)) ||
			(randPos.y <= m_dimensions.y - 3 && (p_Cells[randPos.y + 2][randPos.x].visited == false) && (p_Cells[randPos.y][randPos.x].bot_wall == true && p_Cells[randPos.y + 2][randPos.x].top_wall == true)) ||
			(randPos.x >= 2 && (p_Cells[randPos.y][randPos.x - 2].visited == false) && (p_Cells[randPos.y][randPos.x].left_wall == true && p_Cells[randPos.y][randPos.x - 2].right_wall == true)) ||
			(randPos.x <= m_dimensions.x - 3 && (p_Cells[randPos.y][randPos.x + 2].visited == false) && (p_Cells[randPos.y][randPos.x].right_wall == true && p_Cells[randPos.y][randPos.x + 2].left_wall == true)))
		{
			int random = (m_mtRand() % 4);		// Pick a random wall 0-3 to knock down

												// GO UP
			if ((random == 0) && (randPos.y > 1)) {
				if (p_Cells[randPos.y - 2][randPos.x].visited == false) {	// If not already visited
					p_Cells[randPos.y - 1][randPos.x].display = m_floorChar; // Delete display
					p_Cells[randPos.y - 1][randPos.x].visited = true;		// Mark cell as visited
					p_Cells[randPos.y][randPos.x].top_wall = false;			// Knock down wall

					back_track.push(randPos);									// Push position for back tracking
					just_moved = true;

					randPos.y -= 2;											// Move to next cell
					p_Cells[randPos.y][randPos.x].visited = true;			// Mark cell moved to as visited
					p_Cells[randPos.y][randPos.x].display = m_floorChar;	// Update path
					p_Cells[randPos.y][randPos.x].bot_wall = false;			// Knock down wall
					visitedCells++;												// Increase visitedCells counter
				}
				else
					continue;
			}

			// GO DOWN
			else if ((random == 1) && (randPos.y < m_dimensions.y - 2)) {
				if (p_Cells[randPos.y + 2][randPos.x].visited == false) {	// If not visited
					p_Cells[randPos.y + 1][randPos.x].display = m_floorChar;// Delete display
					p_Cells[randPos.y + 1][randPos.x].visited = true;		// Mark cell as visited
					p_Cells[randPos.y][randPos.x].bot_wall = false;			// Knock down wall

					back_track.push(randPos);									// Push position for back tracking
					just_moved = true;

					randPos.y += 2;											// Move to next cell
					p_Cells[randPos.y][randPos.x].visited = true;			// Mark cell moved to as visited
					p_Cells[randPos.y][randPos.x].display = m_floorChar;	// Update path
					p_Cells[randPos.y][randPos.x].top_wall = false;			// Knock down wall
					visitedCells++;												// Increase visitedCells counter
				}
				else
					continue;
			}

			// GO LEFT
			else if ((random == 2) && (randPos.x > 1)) {
				if (p_Cells[randPos.y][randPos.x - 2].visited == false) {	// If not visited
					p_Cells[randPos.y][randPos.x - 1].display = m_floorChar;// Delete display
					p_Cells[randPos.y][randPos.x - 1].visited = true;		// Mark cell as visited
					p_Cells[randPos.y][randPos.x].left_wall = false;		// Knock down wall

					back_track.push(randPos);									// Push position for back tracking
					just_moved = true;

					randPos.x -= 2;											// Move to next cell
					p_Cells[randPos.y][randPos.x].visited = true;			// Mark cell moved to as visited
					p_Cells[randPos.y][randPos.x].display = m_floorChar;	// Update path
					p_Cells[randPos.y][randPos.x].right_wall = false;		// Knock down wall
					visitedCells++;												// Increase visitedCells counter
				}
				else
					continue;
			}

			// GO RIGHT
			else if ((random == 3) && (randPos.x < m_dimensions.x - 2)) {
				if (p_Cells[randPos.y][randPos.x + 2].visited == false) {	// If not visited
					p_Cells[randPos.y][randPos.x + 1].display = m_floorChar;// Delete display
					p_Cells[randPos.y][randPos.x + 1].visited = true;		// Mark cell as visited
					p_Cells[randPos.y][randPos.x].right_wall = false;		// Knock down wall

					back_track.push(randPos);									// Push position for back tracking
					just_moved = true;

					randPos.x += 2;											// Move to next cell
					p_Cells[randPos.y][randPos.x].visited = true;			// Mark cell moved to as visited
					p_Cells[randPos.y][randPos.x].display = m_floorChar;	// Update path
					p_Cells[randPos.y][randPos.x].left_wall = false;		// Knock down wall
					visitedCells++;												// Increase visitedCells counter
				}
				else
					continue;
			}
		}
		else
		{
			if (just_moved)
			{
				if (m_labelAllEndpoints)
					p_Cells[randPos.y][randPos.x].display = endpointChar++;
				m_endpoints.push_back(randPos);
				just_moved = false;
			}
			randPos = back_track.top();
			back_track.pop();
		}
		if (m_printDuringGeneration)
			printCells();
	}

	m_endpoints.push_back(randPos);
	m_exit = m_endpoints[m_mtRand() % (m_endpoints.size() - 1) + 1];

	p_Cells[m_exit.y][m_exit.x].display = getExitChar();
	p_Cells[Y][X].display = PlayerSimbol;
	for (int i = 0; i < nSnakes; i++)
	{
		p_Cells[Snakes[i].GetSnakeY()][Snakes[i].GetSnakeX()].display = Snakes[i].GetSnakeChar();
	}
	for (int i = 0; i < nCheeze; i++)
	{
		p_Cells[Cheezes[i].GetCheezeY()][Cheezes[i].GetCheezeX()].display = Cheezes[i].GetCheezeChar();
	}
	if (m_printDuringGeneration)
		printCells();
}

void Maze::print(int startCol, int startRow)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// clear screen first
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hOut, &info);
	DWORD d = info.srWindow.Bottom * info.srWindow.Right;
	COORD position = { 0,0 };
	FillConsoleOutputCharacter(hOut, ' ', d, position, &d);

	position.X = (startCol < 0) ? 0 : startCol;
	position.Y = (startRow < 0) ? 0 : startRow;

	for (unsigned int i = 0; i < m_strings.size(); i++)
	{
		SetConsoleCursorPosition(hOut, position);
		std::cout << m_strings[i];
		++position.Y;
	}

}


