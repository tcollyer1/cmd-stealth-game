#include <windows.h>
#include <iostream>

#include "..\h\game.h"

using namespace std;

Map::Map(int enemies, Player* player, Position treasurePos, Position exitPos)
{
	this->enemies		= enemies;
	pPlayer				= player;
	this->treasurePos	= treasurePos;
	this->exitPos		= exitPos;

	//	FOR NOW, hard coded
	height	= 20;
	width	= 50;
}

/// <summary>
/// Draws the map to the screen.
/// </summary>
void Map::Draw()
{
	int i, x, y;
	Position currentPos;

	system("cls");
	for (i = 0; i < width + 2; i++)
	{
		cout << '#';
	}
	cout << "\n";

	for (y = 0; y < height - 1; y++)
	{
		for (x = 0; x < width + 2; x++)
		{
			currentPos.x = x;
			currentPos.y = y;

			if (PlayerAtPosition(currentPos))
			{
				cout << pPlayer->GetSymbol();
			}
			else if ((x == 0) || (x == width + 1))
			{
				cout << '#';
			}
			else
			{
				cout << ' ';
			}
		}
		cout << '\n';
	}

	for (i = 0; i < width + 2; i++)
	{
		cout << '#';
	}
	cout << '\n';
}

bool Map::PlayerAtPosition(Position pos)
{
	return ((pPlayer->GetPosition().x == pos.x) && (pPlayer->GetPosition().y == pos.y));
}

Game::Game()
{
	// Temp - will generate this
	Player* p = new Player(2, 2);
	p->SetSymbol('P');

	Position treasure, exit;

	treasure.x = 10;
	treasure.y = 15;

	exit.x = 20;
	exit.y = 4;

	Map* map = new Map(2, p, treasure, exit);

	pMap = map;

	running = true;
}

void Game::Setup()
{
	pMap->Draw();
}

void Game::Run()
{
	while (running)
	{
		Setup();
		Sleep(1000);
	}
	
}

void Game::ProcessInput()
{
	// Process user input here
}

void Game::EndGame()
{
	// Delete map, entities etc.
}