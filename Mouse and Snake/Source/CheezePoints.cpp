#include "CheezePoints.h"

CheezePoints::CheezePoints() 
{
	CheezeChar = 'V';
	PickedUp = false;
	m_CheezePoints = 0;
	CheezePosY = 15;
	CheezePosX = 33;
}

CheezePoints::CheezePoints(int CheezeY, int CheezeX)
{
	CheezeChar = 'V';
	PickedUp = false;
	m_CheezePoints = 0;
	CheezePosY = CheezeY;
	CheezePosX = CheezeX;
}

void CheezePoints::SetCheezePoints()
{
	m_CheezePoints++;
}

void CheezePoints::GetCheezePoints()
{
	std::cout << "                                         " << std::flush;
	std::cout << "CheezePoints: " << m_CheezePoints << std::endl;
}

int CheezePoints::GetCheezeY()
{
	return CheezePosY;
}

int CheezePoints::GetCheezeX()
{
	return CheezePosX;
}

char CheezePoints::GetCheezeChar()
{
	return CheezeChar;
}


