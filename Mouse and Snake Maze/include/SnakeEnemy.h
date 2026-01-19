#pragma once
class SnakeEnemy
{
	char SnakeChar;
	int Snake_X;
	int Snake_Y;
	int SnakeHealth;
	int DamageAmount;
public:
	SnakeEnemy();
	SnakeEnemy(int Init_Y, int Init_X);
	char GetSnakeChar();
	int GetSnakeX();
	int GetSnakeY();
	void SetSnakeY(int Y);
	void SetSnakeX(int X);
};

