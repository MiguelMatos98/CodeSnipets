#pragma once

#include <windows.h>
#include <chrono>
#include <conio.h>
#include <iostream>
#include <string>
#include <vector>
#include "Vector.h"
#include "SnakeEnemy.h"
#include "CheezePoints.h"
#include <stack>
#include <random>
#include <assert.h>


const int NO_SEED = -1;
const int MINIMUM_SIZE = 8;
const int MAXIMUM_WIDTH = 55;
const int MAXIMUM_HEIGHT = 21;
const int DEFAULT_MAZE_WIDTH = 19;
const int DEFAULT_MAZE_HEIGHT = 19;

class Maze
{
private:
	void initialize();
	void printCells();
	void generateMaze(CheezePoints* Cheezes,SnakeEnemy* Snakes, const int nCheeze,const int nSnakes, char PlayerSimbol, int X, int Y);
	struct Cell
	{
		bool visited;
		bool top_wall;
		bool bot_wall;
		bool left_wall;
		bool right_wall;
		char display;
	};

	Cell** p_Cells;
public:
	double		calcTimeLimit_ms();
	// use createRandomSize() to make a new maze, with random (safe) dimensions and optional random seed
	bool createRandomSize(CheezePoints* Cheezes,SnakeEnemy* Snakes, const int nCheeze,const int nSnakes, int seed = NO_SEED, char PlayerSimbol = 'P', int X = 0, int Y = 0);
	//print() tells the maze to draw itself to the console, once it has been created.
	// specify the upper-left position using the paramters if you want. Default is 0,0.
	void print(int startCol = 0, int startRow = 0);
	// get the character used by the maze to indicate a floor space
	char getFloorChar() { return m_floorChar; }
	// get the character used by the maze to indicate a wall space
	char getWallChar() { return m_wallChar; }
	// get the character used by the maze to indicate the player starting location
	char getStartChar() { return m_startChar; }
	// get the character used by the maze to indicate the player exit location
	char getExitChar() { return m_exitChar; }
	void SetPlayerPos(char PlayerChar, int NewX_Pos, int NewY_Pos, int Current_X, int Current_Y);

	void ClearCheeze(char PlayerChar, int CheezeY, int CheezeX, int NewX_Pos, int NewY_Pos, int Current_X, int Current_Y);
	/*void ResetPlayerPos(char PlayerChar, int NewX_Pos, int NewY_Pos, int CurrentX, int CurrentY);*/

	void SetEnemyPos(char SnakeChar, int NewPosX, int NewPosY, int CurrentX, int CurrentY);

	bool UpdateMaze();

	bool CheckWallChar(int Wall_X, int Wall_Y);

	bool CheckFloorChar(int Wall_X, int Wall_Y);
	// default constructor
	Maze();
	virtual ~Maze();
	// use create() to make a new maze, giving it appropriate dimensions and optional random seed
	bool create(CheezePoints* Cheezes,SnakeEnemy* Snakes, const int nCheeze,const int nSnakes, int seed = NO_SEED, int width = DEFAULT_MAZE_WIDTH, int height = DEFAULT_MAZE_HEIGHT, char PlayerSimbol = 'P', int X = 0, int Y = 0);
	
	Vector m_start{ 0,0 };         // maze starting point
	Vector m_exit{ 0,0 };          // designated end/finish point
	Vector m_dimensions{ 0,0 };    // size of the maze: x=width/columns, y=height/rows
	Vector m_RequestedDimensions{ 0,0 };    // parameters passed to create. different from m_dimensions if the passed values were invalid.
	int m_seed{ 0 };            // seed used to generate the maze
	std::mt19937 m_mtRand;				// mersenne-twister object used for random generation
	char** p_chars{ nullptr };     // pointer to an array of characters - printable maze
	char m_floorChar{ ' ' };      // empty space character
	char m_wallChar{ (char)178 }; // this is a block in the extended ascii table
	char m_startChar{ 'S' };     // starting space character
	char m_exitChar{ 'E' };      // exit space character
	char m_errorChar{ 0 };       // used when querying the maze to report an error
	bool m_printDuringGeneration{ false };  // if true, prints progress while map is generated
	bool m_labelAllEndpoints{ false }; // if true, all corridor ends are labeled with letters, starting with 'a'
	std::vector<std::string>  m_strings; // vector of strings instead that comprise the map
	std::vector<Vector>	m_endpoints; // vector of endpoints: potential start and exit
	double m_msLimit{ 0 };
protected:
	virtual void reset();
};

