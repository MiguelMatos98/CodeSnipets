#include "GameSystem.h"


#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77


GameSystem::GameSystem()
{
	MazeFinder = new Maze;
	Player1 = new Player('M', 100, 2, 3);
	nSnakes = 4;
	nCheeze = 5;
	Snakes = new SnakeEnemy[nSnakes];
	Snakes[0] = SnakeEnemy(1, 3);
	Snakes[1] = SnakeEnemy(5, 3);
	Snakes[2] = SnakeEnemy(1, 39);
	Snakes[3] = SnakeEnemy(15, 39);
	Cheezes = new CheezePoints[nCheeze];
	Cheezes[0] = CheezePoints(15, 33);
	Cheezes[1] = CheezePoints(12, 3);
	Cheezes[2] = CheezePoints(12, 21);
	Cheezes[3] = CheezePoints(1, 26);
	Cheezes[4] = CheezePoints(3, 18);



	EndGame = false;

	MazeFinder->createRandomSize(Cheezes,Snakes, nCheeze,nSnakes, 10, Player1->getCharacter(), Player1->GetX(), Player1->GetY());
}

GameSystem::~GameSystem()
{

	if (Snakes != nullptr)
	{
		delete Snakes;

	}



}

void GameSystem::SetPlayerPos()
{

	/*MazeFinder->SetPlayerPos(Player1->getCharacter(), Player1->GetX(), Player1->GetY());*/

	int PlayerInput;
	//cout << endl;
	//cout << "Where whould you like to move using W,A,S,D: " << endl;
	switch ((PlayerInput = _getch()))
	{
	case KEY_UP:
		if (MazeFinder->CheckFloorChar(Player1->GetX(), Player1->GetY() - 1))
		{
			MazeFinder->SetPlayerPos(Player1->getCharacter(), Player1->GetX(), Player1->GetY() - 1, Player1->GetX(), Player1->GetY());
			Player1->SetNegativeY();

			
		}
		else if (((Player1->GetY() - 1) == Snakes->GetSnakeY() && Player1->GetY() != Snakes->GetSnakeY()) && Player1->GetX() == Snakes->GetSnakeX())
		{
			Player1->UpdateHealth(10);
			Player1->GetHealth();
			Sleep(2500);
		}
		else
		{
			for (int i = 0; i < nCheeze; i++)
			{
				if ((Player1->GetY() - 1) == Cheezes[i].GetCheezeY() && Player1->GetY() != Cheezes[i].GetCheezeY() && Player1->GetX() == Cheezes[i].GetCheezeX())
				{
					MazeFinder->ClearCheeze(Player1->getCharacter(), Cheezes->GetCheezeY(), Cheezes->GetCheezeX(), Player1->GetX(), Player1->GetY() - 1, Player1->GetX(), Player1->GetY());
					Player1->SetNegativeY();
					Cheezes->SetCheezePoints();
					Cheezes->GetCheezePoints();
					Sleep(2500);
				}
			}
		}
		break;
	case KEY_LEFT:
		if (MazeFinder->CheckFloorChar(Player1->GetX() - 1, Player1->GetY()))
		{
			MazeFinder->SetPlayerPos(Player1->getCharacter(), Player1->GetX() - 1, Player1->GetY(), Player1->GetX(), Player1->GetY());
			Player1->SetNegativeX();
			
		}
		else if (Player1->GetY() == Snakes->GetSnakeY() && (Player1->GetX() - 1) == Snakes->GetSnakeX() && Player1->GetX() != Snakes->GetSnakeX())
		{
			Player1->UpdateHealth(10);
			Player1->GetHealth();
			Sleep(2500);
		}
		else
		{
			for (int i = 0; i < nCheeze; i++)
			{
				if ((Player1->GetX() - 1) == Cheezes[i].GetCheezeX() && Player1->GetX() != Cheezes[i].GetCheezeX() && Player1->GetY() == Cheezes[i].GetCheezeY())
				{
					MazeFinder->ClearCheeze(Player1->getCharacter(), Cheezes->GetCheezeY(), Cheezes->GetCheezeX(), Player1->GetX() - 1, Player1->GetY(), Player1->GetX(), Player1->GetY());
					Player1->SetNegativeX();
					Cheezes->SetCheezePoints();
					Cheezes->GetCheezePoints();
					Sleep(2500);
				}
			}
		}
		
		break;
	case KEY_DOWN:
		if (MazeFinder->CheckFloorChar(Player1->GetX(), Player1->GetY() + 1))
		{
			MazeFinder->SetPlayerPos(Player1->getCharacter(), Player1->GetX(), Player1->GetY() + 1, Player1->GetX(), Player1->GetY());
			Player1->SetPositiveY();
		}
		else if (((Player1->GetY() + 1) == Snakes->GetSnakeY() && Player1->GetY() != Snakes->GetSnakeY()) && Player1->GetX() == Snakes->GetSnakeX())
		{
			Player1->UpdateHealth(10);
			Player1->GetHealth();
			Sleep(2500);
		}
		else
		{
			for (int i = 0; i < nCheeze; i++)
			{
				if ((Player1->GetY() + 1) == Cheezes[i].GetCheezeY() && Player1->GetY() != Cheezes[i].GetCheezeY() && Player1->GetX() == Cheezes[i].GetCheezeX())
				{
					MazeFinder->ClearCheeze(Player1->getCharacter(), Cheezes->GetCheezeY(), Cheezes->GetCheezeX(), Player1->GetX(), Player1->GetY() + 1, Player1->GetX(), Player1->GetY());
					Player1->SetPositiveY();
					Cheezes->SetCheezePoints();
					Cheezes->GetCheezePoints();
					Sleep(2500);
				}
			}
		}
		break;
	case KEY_RIGHT:
		if (MazeFinder->CheckFloorChar(Player1->GetX() + 1, Player1->GetY()))
		{
			MazeFinder->SetPlayerPos(Player1->getCharacter(), Player1->GetX() + 1, Player1->GetY(), Player1->GetX(), Player1->GetY());
			Player1->SetpositiveX();
		}
		else if (Player1->GetY() == Snakes->GetSnakeY() && ((Player1->GetX() + 1) == Snakes->GetSnakeX() && Player1->GetX() != Snakes->GetSnakeX()))
		{
			Player1->UpdateHealth(10);
			Player1->GetHealth();
			Sleep(2500);
		}
		else
		{
			for (int i = 0; i < nCheeze; i++)
			{
				if ((Player1->GetX() + 1) == Cheezes[i].GetCheezeX() && Player1->GetX() != Cheezes[i].GetCheezeX() && Player1->GetY() == Cheezes[i].GetCheezeY())
				{
					MazeFinder->ClearCheeze(Player1->getCharacter(), Cheezes->GetCheezeY(), Cheezes->GetCheezeX(), Player1->GetX() + 1, Player1->GetY(), Player1->GetX(), Player1->GetY());
					Player1->SetpositiveX();
					Cheezes->SetCheezePoints();
					Cheezes->GetCheezePoints();
					Sleep(2500);
				}
			}
		}
		break;
	}


}

void GameSystem::PrintMaze()
{
	while (true)
	{
		MazeFinder->print(1, 1);
		Sleep(1700);
		system("cls");
	}
		
		
	
}

void GameSystem::SnakeMovement()
{
	int RandomMov;

	Sleep(3000);

	while (!EndGame)
	{
		for (int i = 0; i < nSnakes; i++) {
			srand(time(NULL));
			RandomMov = rand() % 4 + 1;
			if (RandomMov == 1)
			{
				if (MazeFinder->CheckFloorChar(Snakes[i].GetSnakeX(), Snakes[i].GetSnakeY() - 1) && !(MazeFinder->CheckWallChar(Snakes[i].GetSnakeX(), Snakes[i].GetSnakeY() - 1)) && ((Snakes[i].GetSnakeY()-1) != Player1->GetY() || Snakes[i].GetSnakeY() - 2) != Player1->GetY())
				{
					if (((Snakes[i].GetSnakeY() - 1) != Player1->GetY() || Snakes[i].GetSnakeY() - 2) != Player1->GetY())
					{
						MazeFinder->SetEnemyPos(Snakes[i].GetSnakeChar(), Snakes[i].GetSnakeX(), Snakes[i].GetSnakeY() - 2, Snakes[i].GetSnakeX(), Snakes[i].GetSnakeY());
						Snakes[i].SetSnakeY(-2);
					}
					
				}

			}
			else if (RandomMov == 2)
			{
				if (MazeFinder->CheckFloorChar(Snakes[i].GetSnakeX() + 1, Snakes[i].GetSnakeY()) && !(MazeFinder->CheckWallChar(Snakes[i].GetSnakeX() + 1, Snakes[i].GetSnakeY())) && ((Snakes[i].GetSnakeX() + 1) != Player1->GetY() || Snakes[i].GetSnakeX() + 2) != Player1->GetY())
				{
					if (((Snakes[i].GetSnakeX() + 1) != Player1->GetX() || Snakes[i].GetSnakeX() + 2) != Player1->GetX())
					{
						MazeFinder->SetEnemyPos(Snakes[i].GetSnakeChar(), Snakes[i].GetSnakeX() + 2, Snakes[i].GetSnakeY(), Snakes[i].GetSnakeX(), Snakes[i].GetSnakeY());
						Snakes[i].SetSnakeX(2);
					}
					
				}
			}
			else if (RandomMov == 3)
			{
				if (MazeFinder->CheckFloorChar(Snakes[i].GetSnakeX(), Snakes[i].GetSnakeY() + 1) && !(MazeFinder->CheckWallChar(Snakes[i].GetSnakeX(), Snakes[i].GetSnakeY() + 1)))
				{
					if (((Snakes[i].GetSnakeY() + 1) != Player1->GetY() || Snakes[i].GetSnakeY() + 2) != Player1->GetY())
					{
						MazeFinder->SetEnemyPos(Snakes[i].GetSnakeChar(), Snakes[i].GetSnakeX(), Snakes[i].GetSnakeY() + 2, Snakes[i].GetSnakeX(), Snakes[i].GetSnakeY());
						Snakes[i].SetSnakeY(2);
					}
				}
			}
			else
			{
				if (MazeFinder->CheckFloorChar(Snakes[i].GetSnakeX() - 1, Snakes[i].GetSnakeY()) && !(MazeFinder->CheckWallChar(Snakes[i].GetSnakeX() - 1, Snakes[i].GetSnakeY())) )
				{
					if (((Snakes[i].GetSnakeX() - 1) != Player1->GetX() || Snakes[i].GetSnakeX() - 2) != Player1->GetX())
					{
						MazeFinder->SetEnemyPos(Snakes[i].GetSnakeChar(), Snakes[i].GetSnakeX() - 2, Snakes[i].GetSnakeY(), Snakes[i].GetSnakeX(), Snakes[i].GetSnakeY());
						Snakes[i].SetSnakeX(-2);
					}
				}
			}
			MazeFinder->UpdateMaze();
			Sleep(1500);
		}
	}

}

void GameSystem::StartGame()
{
	//SnakeMovement_Thrd Smt;
	thread thread_obj(&GameSystem::SnakeMovement, this);
	thread thread_Player(&GameSystem::PrintMaze, this);
	
	while (!EndGame)
	{
		SetPlayerPos();

	}
		

		//SetPlayerPos();
		/*MazeFinder->UpdateMaze();*/
		/*thread_obj.join();
		thread_Player.join();*/
}

bool GameSystem::GetEndGame()
{
	return EndGame;
}