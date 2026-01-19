#pragma once
#include "CheezePoints.h"
class Player
{
	int m_Health;
	char m_char;
	int m_X;
	int m_Y;
public:
	Player();
	Player(char PlayerChar, int Health, int X, int Y);
	char getCharacter();
	void SetpositiveX();
	void SetNegativeX();
	void SetPositiveY();
	void SetNegativeY();
	int GetX();
	int GetY();
	void GetHealth();
	void UpdateHealth(int Damage);


};

