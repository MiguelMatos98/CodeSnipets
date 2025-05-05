#include "SnakeEnemy.h"

SnakeEnemy::SnakeEnemy()
{
	SnakeChar = '$';
	Snake_X = 3;
	Snake_Y = 1;
	SnakeHealth = 70;
	DamageAmount = 10;
}

SnakeEnemy::SnakeEnemy(int Init_Y, int Init_X)
{
	SnakeChar = '$';
	Snake_X = Init_X;
	Snake_Y = Init_Y;
	SnakeHealth = 70;
	DamageAmount = 10;
}

char SnakeEnemy::GetSnakeChar()
{
	return SnakeChar;
}

int SnakeEnemy::GetSnakeX()
{
	return Snake_X;
}

int SnakeEnemy::GetSnakeY()
{
	return Snake_Y;
}

void SnakeEnemy::SetSnakeY(int Y)
{
	Snake_Y += Y;
}

void SnakeEnemy::SetSnakeX(int X)
{
	Snake_X += X;
}
