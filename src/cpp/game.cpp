#include <windows.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <string>

#include "..\h\game.h"
#include "..\h\env.h"

static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); // Global for standard reuse

GameMap::GameMap(int enemies, Player* player, Treasure* treasure, Exit* exit)
{
	numEnemies	= enemies;
	pPlayer		= player;
	pTreasure	= treasure;
	pExit		= exit;

	int y, x;
	Entity::Position tempPos;
	bool positionAdded;

	vector<Entity::Position> usedPositions;

	// Add player, treasure and exit to vector of all map entities
	entities.push_back(pPlayer);
	entities.push_back(pTreasure);
	entities.push_back(pExit);

	// Add all positions currently in entities vector to usedPositions so they cannot be reused for enemies
	for (int i = 0; i < entities.size(); i++)
	{
		usedPositions.push_back(entities[i]->GetPosition());
	}

	int startSize = usedPositions.size();

	for (int i = 0; i < enemies; i++)
	{
		positionAdded = false;

		while (!positionAdded)
		{
			y = (rand() % height) + 1;
			x = (rand() % width) + 1;

			tempPos.x = x;
			tempPos.y = y;

			if (usedPositions.size() == startSize)
			{
				Enemy* e = new Enemy(x, y, true); // Give one enemy (first enemy) a key
				this->enemies.push_back(e);
				entities.push_back(e);

				usedPositions.push_back(tempPos);

				positionAdded = true;
			}
			else if (find(usedPositions.begin(), usedPositions.end(), tempPos) == usedPositions.end()) // find() points to the last element if not found
			{
				Enemy* e = new Enemy(x, y, false);
				this->enemies.push_back(e);
				entities.push_back(e);

				usedPositions.push_back(tempPos);

				positionAdded = true;
			}
			else
			{
			}
		}
		
	}

	/*Enemy* e1 = new Enemy(rand() % width, rand() % height, false);
	Enemy* e2 = new Enemy(rand() % width, rand() % height, true);*/

	/*this->enemies.push_back(e1);
	this->enemies.push_back(e2);*/

	/*entities.push_back(e1);
	entities.push_back(e2);*/
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
	Entity::Position currentPos;

	currentPos.x = 0;
	currentPos.y = 0;

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

	// Temp - will generate this
	Player*		p = new Player(2, 19);
	Treasure*	t = new Treasure(10, 15);
	Exit*		e = new Exit(20, 4);

	GameMap* map = new GameMap(3, p, t, e);

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
		default:
			break;
		}
	}
}

void Game::DisplayText(wstring text, int lineNo, int colour)
{
	COORD coords = { 0, GameMap::height + 2 + lineNo };

	SetConsoleCursorPosition(handle, coords);
	SetConsoleTextAttribute(handle, colour);

	wcout << text;

	SetConsoleTextAttribute(handle, 7); // Reset colour
}

void Game::ShowHelp()
{
	system("cls");
	wifstream helpTxt("help.txt");
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