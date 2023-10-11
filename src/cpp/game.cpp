#include <windows.h>
#include <iostream>
#include <conio.h>

#include "..\h\game.h"
#include "..\h\env.h"

static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); // Global for standard reuse

GameMap::GameMap(int enemies, Player* player, Treasure* treasure, Exit* exit)
{
	numEnemies	= enemies;
	pPlayer		= player;
	pTreasure	= treasure;
	pExit		= exit;

	// Will use int enemies to determine how many to generate and generate (semi-) random positions - this is all temp
	Enemy* e1 = new Enemy(40, 5, false);
	Enemy* e2 = new Enemy(42, 12, true);

	this->enemies.push_back(e1);
	this->enemies.push_back(e2);

	entities.push_back(e1);
	entities.push_back(e2);

	height	= GAME_HEIGHT;
	width	= GAME_WIDTH;
}

/// <summary>
/// Writes entities to the map.
/// </summary>
/// <param name="entity">Pointer to the entity to write</param>
void GameMap::WriteEntity(Entity* entity)
{	
	cout.flush();

	COORD coords = { (SHORT)entity->GetPosition().x, (SHORT)entity->GetPosition().y };

	SetConsoleCursorPosition(handle, coords);
	SetConsoleTextAttribute(handle, entity->GetColour());

	entity->DrawEntity();
}

void GameMap::SetUpMap()
{
	int i, x, y;
	Entity::Position currentPos;

	currentPos.x = 0;
	currentPos.y = 0;

	entities.push_back(pPlayer);
	entities.push_back(pTreasure);
	entities.push_back(pExit);

	system("cls");
	for (i = 0; i < width + 2; i++)	// +2 for left/right map boundary
	{
		currentPos.x = i;
		Wall* wall = new Wall(currentPos.x, currentPos.y);
		WriteEntity(wall);

		walls.push_back(wall);

		entities.push_back(wall);
	}
	wcout << "\n";

	for (y = 1; y <= height; y++)
	{
		for (x = 0; x < width + 2; x++)
		{
			currentPos.x = x;
			currentPos.y = y;

			if ((x == 0) || (x == width + 1))
			{
				Wall* wall = new Wall(currentPos.x, currentPos.y);
				WriteEntity(wall);

				walls.push_back(wall);

				entities.push_back(wall);
			}
			else
			{
				Tile* tile = new Tile(currentPos, Tile::HARD, Tile::BRIGHT);
				WriteEntity(tile);

				tiles.push_back(tile);

				entities.push_back(tile);
			}			
		}
		wcout << '\n';
	}

	currentPos.y = height + 1;	// +1 for top map boundary

	for (i = 0; i < width + 2; i++)
	{
		currentPos.x = i;

		Wall* wall = new Wall(currentPos.x, currentPos.y);
		WriteEntity(wall);

		walls.push_back(wall);

		entities.push_back(wall);
	}
	wcout << '\n';


}

/// <summary>
/// Draws the map content to the screen.
/// </summary>
void GameMap::DrawContent()
{
	WriteEntity(pPlayer);
	WriteEntity(pTreasure);
	WriteEntity(pExit);

	for (int i = 0; i < enemies.size(); i++)
	{
		WriteEntity(enemies[i]);
	}
}

void GameMap::RequestMove(Character::Movement move)
{
	Entity::Position oldPos = pPlayer->GetPosition();
	Entity::Position newPos = pPlayer->CalculatePos(move);

	int		i		= 0;
	bool	found	= false;

	if (GetIfTraversable(newPos))
	{
		// Player must previously have been on a walkable tile, so therefore replace it with whatever existing tile was there
		while (i < tiles.size() && !found)
		{
			if (tiles[i]->GetPosition() == oldPos)
			{
				WriteEntity(tiles[i]);
				found = true;
			}

			i++;
		}

		pPlayer->Move(newPos);
		WriteEntity(pPlayer);
	}
}

/// <summary>
/// Returns whether a Character can move to a specified position or not.
/// </summary>
/// <param name="pos">Desired position</param>
/// <returns>True/false</returns>
bool GameMap::GetIfTraversable(Entity::Position pos)
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

		i++;
	}

	return (traversable);
}

Game::Game()
{
	// Make the console cursor invisible
	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(handle, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(handle, &cursorInfo);

	// Temp - will generate this
	Player*		p = new Player(2, 19);
	Treasure*	t = new Treasure(10, 15);
	Exit*		e = new Exit(20, 4);

	GameMap* map = new GameMap(2, p, t, e);

	pMap = map;

	running = true;
}

void Game::GameLoop()
{
	pMap->DrawContent(); // Update map
}

/// <summary>
/// Everything needed to run the game - the main entry point.
/// </summary>
void Game::Run()
{
	pMap->SetUpMap();

	while (running)
	{
		GameLoop();
		ProcessInput();
		Sleep(50);
	}
	
}

void Game::ProcessInput()
{
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
		case 'e':
			EndGame();
			break;
		default:
			break;
		}
	}
}

void Game::EndGame()
{
	// Delete map, entities etc.
	running = false;
	system("cls");
	wcout << "Thanks for playing!";
}