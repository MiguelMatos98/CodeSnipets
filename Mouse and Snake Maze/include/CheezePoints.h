#pragma once
#include <iostream>
class CheezePoints
{
	char CheezeChar;
	bool PickedUp;
	int m_CheezePoints;
	int CheezePosY;
	int CheezePosX;
public:
	CheezePoints();
	CheezePoints(int CheezeY, int CheezeX);
	void SetCheezePoints();
	void GetCheezePoints();
	int GetCheezeY();
	int GetCheezeX();
	char GetCheezeChar();
};

