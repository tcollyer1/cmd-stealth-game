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

void Map::SetUpMap()
{
	int i, x, y;
	Entity::Position currentPos;

	currentPos.x = 0;
	currentPos.y = 0;

	Wall wall = Wall(0,0);
	Tile tile = Tile(currentPos, true, Tile::HARD, Tile::BRIGHT);

	entities.push_back(pPlayer);
	entities.push_back(pTreasure);
	entities.push_back(pExit);

	system("cls");
	for (i = 0; i < width + 2; i++)
	{
		wall = Wall(0, i);
		currentPos.y = i;
		wall.DrawEntity();

		walls.push_back(wall);

		entities.push_back(&wall);	//	This will NOT work... Needs to be pointers
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

				entities.push_back(&wall);
			}
			else
			{
				tile = Tile(currentPos, true, Tile::HARD, Tile::BRIGHT);
				tile.DrawEntity();

				tiles.push_back(tile);

				entities.push_back(&tile);
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

		entities.push_back(&wall);
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

void Map::RequestMove(Character::Movement move)
{
	Entity::Position pos = pPlayer->CalculatePos(move);

	if (GetIfTraversable(pos))
	{
		pPlayer->Move(pos);
	}
}

/// <summary>
/// Returns whether a Character can move to a specified position or not.
/// </summary>
/// <param name="pos">Desired position</param>
/// <returns>True/false</returns>
bool Map::GetIfTraversable(Entity::Position pos)
{
	bool traversable	= false;
	bool found			= false;
	int i = 0;

	while (i < (int)entities.size() && !found)
	{
		if (entities[i]->GetPosition() == pos)
		{
			traversable = entities[i]->GetIfPassable();
			found = true;
		}
	}

	//if (pPlayer->GetPosition() == pos)
	//{
	//	traversable = pPlayer->GetIfPassable();
	//}

	//else if (pTreasure->GetPosition() == pos)
	//{
	//	traversable = pTreasure->GetIfPassable();
	//}

	//else if (pExit->GetPosition() == pos)
	//{
	//	traversable = pExit->GetIfPassable();
	//}

	//else
	//{
	//	int i = 0;
	//	while (i < walls.size() && !found)
	//	{
	//		if (walls[i].GetPosition() == pos)
	//		{
	//			traversable = walls[i].GetIfPassable();
	//			found = true;
	//		}

	//		i++;
	//	}

	//	i = 0;

	//	// Check enemies before tiles - enemies can temporarily overwrite tiles
	//	while (i < enemies.size() && !found)
	//	{
	//		if (enemies[i].GetPosition() == pos)
	//		{
	//			traversable = enemies[i].GetIfPassable();
	//			found = true;
	//		}

	//		i++;
	//	}

	//	i = 0;

	//	while (i < tiles.size() && !found)
	//	{
	//		if (tiles[i].GetPosition() == pos)
	//		{
	//			traversable = tiles[i].GetIfPassable();
	//			found = true;
	//		}

	//		i++;
	//	}
	//}

	return (traversable);
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
	pMap->DrawContent();
}

void Game::Run()
{
	pMap->SetUpMap();

	while (running)
	{
		Setup();
		ProcessInput();
		Sleep(1000);
	}
	
}

void Game::ProcessInput()
{
	// Process user input here
	if (_kbhit())
	{
		switch (_getch())
		{
		case 'w':
			pMap->RequestMove(Character::UP);
			break;
		case 's':
			pMap->RequestMove(Character::DOWN);
			break;
		case 'a':
			pMap->RequestMove(Character::LEFT);
			break;
		case 'd':
			pMap->RequestMove(Character::RIGHT);
			break;
		default:
			break;
		}
	}
}

void Game::EndGame()
{
	// Delete map, entities etc.
}