#include <windows.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <string>

#include "..\h\game.h"
#include "..\h\env.h"

static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); // Global for standard reuse

GameMap::GameMap(int enemies)
{
	numEnemies	= enemies;

	// Temp - will generate these
	pPlayer		= new Player(2, 19);
	pTreasure	= new Treasure(10, 15);
	pExit		= new Exit(20, 4);
	Enemy* e	= new Enemy(3, 4, true); // Must have at least one enemy, which will have the key

	int y, x;
	Entity::Position tempPos;
	bool positionAdded;

	vector<Entity::Position> usedPositions;

	// Add player, treasure and exit to vector of all map entities
	entities.push_back(pPlayer);
	entities.push_back(pTreasure);
	entities.push_back(pExit);

	this->enemies.push_back(e);
	entities.push_back(e);

	// Populate map with enemies and gold
	AddEntities<Enemy>(numEnemies - 1, this->enemies); // -1 for the one enemy already on the map
	AddEntities<Gold>(5, gold);
}

/// <summary>
/// Template function that adds a certain amount of a particular type of entity to the map at random.
/// </summary>
/// <typeparam name="T">The Entity object type to add</typeparam>
/// <param name="num">Number of this type of entity to add to the map</param>
/// <param name="entitiesVector">The vector that holds all of the data for this type of entity</param>
template<typename T>
void GameMap::AddEntities(int num, vector<T*> &entitiesVector)
{
	bool				positionAdded;
	Entity::Position	tempPos;
	int					y, x;

	vector<Entity::Position> usedPositions;

	// Add all positions currently in entities vector to usedPositions so they cannot be reused for enemies
	for (int i = 0; i < entities.size(); i++)
	{
		usedPositions.push_back(entities[i]->GetPosition());
	}

	for (int i = 0; i < num; i++)
	{
		positionAdded = false;

		while (!positionAdded)
		{
			y = (rand() % height) + 1;
			x = (rand() % width) + 1;

			tempPos.x = x;
			tempPos.y = y;

			if (find(usedPositions.begin(), usedPositions.end(), tempPos) == usedPositions.end()) // find() points to the last element if not found
			{
				T* e = new T(x, y);
				entitiesVector.push_back(e);
				entities.push_back(e);

				usedPositions.push_back(tempPos);

				positionAdded = true;
			}
			else
			{
				// Continue finding a position
			}
		}

	}
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

	SetConsoleTextAttribute(handle, 7); // Reset colour
}

void GameMap::SetUpMap()
{
	int i, x, y;

	system("cls");
	for (i = 0; i < width + 2; i++)	// +2 for left/right map boundary
	{
		Wall* wall = new Wall(i, 0);
		WriteEntity(wall);

		walls.push_back(wall);

		entities.push_back(wall);
	}
	wcout << "\n";

	for (y = 1; y <= height; y++)
	{
		for (x = 0; x < width + 2; x++)
		{
			if ((x == 0) || (x == width + 1))
			{
				Wall* wall = new Wall(x, y);
				WriteEntity(wall);

				walls.push_back(wall);

				entities.push_back(wall);
			}
			else
			{
				Tile* tile = new Tile(x, y, Tile::HARD, Tile::BRIGHT);
				WriteEntity(tile);

				tiles.push_back(tile);

				entities.push_back(tile);
			}			
		}
		wcout << '\n';
	}

	for (i = 0; i < width + 2; i++)
	{
		Wall* wall = new Wall(i, height + 1); // +1 for top map boundary
		WriteEntity(wall);

		walls.push_back(wall);

		entities.push_back(wall);
	}
	wcout << '\n';


}

void GameMap::RedrawMap()
{
	system("cls");

	for (int x = 0; x < entities.size(); x++)
	{
		WriteEntity(entities[x]);
	}
}

/// <summary>
/// Draws the current map content to the screen on refresh.
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

	for (int i = 0; i < gold.size(); i++)
	{
		WriteEntity(gold[i]);
	}
}

void GameMap::RequestPlayerMove(Character::Movement move)
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
		//WriteEntity(pPlayer);
	}
}

int GameMap::RequestGoldPickup()
{
	Entity::Position playerPos = pPlayer->GetPosition();

	int		i		= 0;
	bool	found	= false;

	int		value	= 0;

	while (i < gold.size() && !found)
	{
		if (gold[i]->GetPosition() == playerPos)
		{
			value = gold[i]->GetValue();
			Gold* temp = gold[i];

			gold.erase(gold.begin() + i);
			temp->Destroy();
			//gold[i]->Destroy(); // This needs to be handled much better

			found = true;
		}

		i++;
	}

	pPlayer->IncrementGold(value);

	return (value);
}

// Possibly merge this with RequestPlayerMove() somehow...
void GameMap::MoveEnemy(Character::Movement move, Enemy* enemy)
{
	Entity::Position oldPos = enemy->GetPosition();
	Entity::Position newPos = enemy->CalculatePos(move);

	int		i = 0;
	bool	found = false;

	if (GetIfTraversable(newPos))
	{
		// Enemy must previously have been on a walkable tile, so therefore replace it with whatever existing tile was there
		while (i < tiles.size() && !found)
		{
			if (tiles[i]->GetPosition() == oldPos)
			{
				WriteEntity(tiles[i]);
				found = true;
			}

			i++;
		}

		enemy->Move(newPos, move); // Needs to change so that this func can be merged with RequestPlayerMove()
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

	GameMap* map = new GameMap(3);

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
	DisplayText(L"H - Show Help", 1, 10);
	DisplayText(L"Gold:  0  ", 2, 6);

	while (running)
	{
		GameLoop();
		ProcessInput();
		Sleep(50);
	}
	
}

void Game::ProcessInput()
{
	int gold = 0;

	if (_kbhit())
	{
		switch (_getch())
		{
		case 'w':
			pMap->RequestPlayerMove(Character::UP);
			break;
		case 's':
			pMap->RequestPlayerMove(Character::DOWN);
			break;
		case 'a':
			pMap->RequestPlayerMove(Character::LEFT);
			break;
		case 'd':
			pMap->RequestPlayerMove(Character::RIGHT);
			break;
		case 'e':
			EndGame();
			break;
		case 'h':
			ShowHelp();
			break;
		case 32: // Space
			// Prioritise checking if player is behind enemy FIRST (for takedown)
			// Otherwise just pick up gold if there's any there
			gold = pMap->RequestGoldPickup();
			if (gold)
			{
				wstring str = L"Gold:  " + gold;
				DisplayText(str, 2, 6);
			}
			break;
		default:
			break;
		}
	}
}

void Game::DisplayText(wstring text, int lineNo, int colour) // This function is crap
{
	COORD coords = { 0, GameMap::height + 2 + lineNo };

	SetConsoleCursorPosition(handle, coords);
	SetConsoleTextAttribute(handle, colour);

	wcout << text << "\n";

	SetConsoleTextAttribute(handle, 7); // Reset colour
}

void Game::ShowHelp()
{
	system("cls");
	wifstream helpTxt("help.txt"); // Wrap this in a try-catch in case file is deleted
	wstring txt;
	bool close = false;

	while (getline(helpTxt, txt))
	{
		wcout << txt << "\n";
	}

	helpTxt.close();

	wcout << "\n\n> Press the ENTER key to continue...";

	while (!close)
	{
		if (_kbhit())
		{
			if (_getch() == 13) // Enter
			{
				close = true;
			}
		}
	}

	pMap->RedrawMap();
	DisplayText(L"H - Show Help", 1, 10);
}

void Game::EndGame()
{
	// Delete map, entities etc.
	running = false;
	system("cls");
	wcout << "Thanks for playing!";
}