#include "Player.h"
#include <iostream>
Player::Player()
{
	m_char = 'M';
	m_X = 11;
	m_Y = 2;
	m_Health = 100;
}

Player::Player(char PlayerChar, int Health, int X, int Y)
{
	m_char = PlayerChar;
	m_X = X;
	m_Y = Y;
	m_Health = Health;
}


char Player::getCharacter()
{
	return m_char;
}

void Player::SetpositiveX()
{
	++m_X;

}

void Player::SetNegativeX()
{
	--m_X;

}
void Player::SetPositiveY()
{
	++m_Y;
}

void Player::SetNegativeY()
{
	--m_Y;
}

int Player::GetX()
{
	return m_X;
}

int Player::GetY()
{
	return m_Y;
}

void Player::GetHealth()
{
				  
	std::cout << "                                         " << std::flush;
	std::cout << "Health: " << m_Health << std::endl;
}

void Player::UpdateHealth(int Damage)
{
	m_Health -= Damage;
}