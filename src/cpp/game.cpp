#include <windows.h>
#include <iostream>
#include <conio.h>

#include "..\h\game.h"
#include "..\h\env.h"

Map::Map(int enemies, Player* player, Treasure* treasure, Exit* exit)
{
	numEnemies	= enemies;
	pPlayer		= player;
	pTreasure	= treasure;
	pExit		= exit;

	// FOR NOW, hard coded
	height	= GAME_HEIGHT;
	width	= GAME_WIDTH;
}

/// <summary>
/// Writes entities to the map.
/// </summary>
/// <param name="entity">Pointer to the entity to write</param>
void Map::WriteEntity(Entity* entity)
{
	static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	cout.flush();

	COORD coords = { (SHORT)entity->GetPosition().x, (SHORT)entity->GetPosition().y };
	SetConsoleCursorPosition(handle, coords);
	SetConsoleTextAttribute(handle, entity->GetColour());

	entity->DrawEntity();

	SetConsoleTextAttribute(handle, 7);
}

void Map::DrawEmptyMap()
{
	int i, x, y;
	Position currentPos;

	currentPos.x = 0;
	currentPos.y = 0;

	Wall wall = Wall(0,0);
	Tile tile = Tile(currentPos, true, Tile::HARD, Tile::BRIGHT);

	system("cls");
	for (i = 0; i < width + 2; i++)
	{
		wall = Wall(0, i);
		currentPos.y = i;
		wall.DrawEntity();

		walls.push_back(wall);
	}
	cout << "\n";

	for (y = 0; y < height - 1; y++)
	{
		for (x = 0; x < width + 2; x++)
		{
			currentPos.x = x;
			currentPos.y = y;

			if ((x == 0) || (x == width + 1))
			{
				wall = Wall(currentPos.x, currentPos.y);
				wall.DrawEntity();

				walls.push_back(wall);
			}
			else
			{
				tile = Tile(currentPos, true, Tile::HARD, Tile::BRIGHT);
				tile.DrawEntity();

				tiles.push_back(tile);
			}			
		}
		cout << '\n';
	}

	for (i = 0; i < width + 2; i++)
	{
		wall = Wall(0, i);
		currentPos.y = i;
		wall.DrawEntity();

		walls.push_back(wall);
	}
	cout << '\n';
}

/// <summary>
/// Draws the map content to the screen.
/// </summary>
void Map::DrawContent()
{
	WriteEntity(pPlayer);
	WriteEntity(pTreasure);
	WriteEntity(pExit);
}

Game::Game()
{
	// Temp - will generate this
	Player*		p = new Player(5, 2);
	Treasure*	t = new Treasure(10, 15);
	Exit*		e = new Exit(20, 4);

	Map* map = new Map(2, p, t, e);

	pMap = map;

	running = true;
}

void Game::Setup()
{
	pMap->DrawEmptyMap();
	pMap->DrawContent();
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