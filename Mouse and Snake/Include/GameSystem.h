#pragma once

#include <conio.h>
#include <stdlib.h>
#include <iostream>
#include "Player.h"
#include "SnakeEnemy.h"
#include "Maze.h"
#include <thread>
using namespace std;

class GameSystem
{
private:
	bool EndGame;
	SnakeEnemy* Snakes;
	Maze* MazeFinder;
	Player* Player1;
	CheezePoints* Cheezes;
	int nSnakes;
	int nCheeze;
public:
	GameSystem();
	~GameSystem();
	void SetPlayerPos();
	void PrintMaze();
	void StartGame();
	bool GetEndGame();
	void SnakeMovement();


};

