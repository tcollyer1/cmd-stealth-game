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
void GameMap::WriteEntity(Entity* entity, int background)
{	
	cout.flush();

	COORD coords = { (SHORT)entity->GetPosition().x, (SHORT)entity->GetPosition().y };

	int fg = entity->GetColour().foreground;
	int bg = entity->GetColour().background;

	if (background != 0)
	{
		bg = background; // Overwrite background with new
	}

	int colour = Game::GetColourCode(fg, bg);

	SetConsoleCursorPosition(handle, coords);
	SetConsoleTextAttribute(handle, colour);

	entity->DrawEntity();

	SetConsoleTextAttribute(handle, Entity::DARK_WHITE); // Reset colour
}

void GameMap::SetUpMap()
{
	int i, x, y;

	Tile::LightLevel	light;
	Tile::TerrainType	terrain;

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
				if ((x != pPlayer->GetPosition().x) && (y != pPlayer->GetPosition().y))
				{
					light = Tile::LightLevel(rand() % 3);
					terrain = Tile::TerrainType(rand() % 2);
				}
				// Always spawn player initially on a dark tile to prevent unfairness on a new game
				else
				{
					light	= Tile::LightLevel::DARK;
					terrain = Tile::TerrainType::SOFT;
				}

				Tile* tile = new Tile(x, y, terrain, light); // To be generated SEMI-randomly
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

	Game::DisplayText(L"H - Show Help", Game::hintLineNo, 10, true);
	Game::DisplayText(L"Gold:  " + to_wstring(pPlayer->GetGold()), Game::goldLineNo, 6, true);

	if (pPlayer->GetKeyObtained())
	{
		Game::DisplayText(L"KEY OBTAINED", Game::progressLineNo, 22, true);
	}	
}

/// <summary>
/// Draws the current map content to the screen on refresh.
/// </summary>
void GameMap::DrawContent()
{
	int bg, playerBg, treasureBg, exitBg = 0; // TODO: improve this function

	playerBg	= GetTileBackground(pPlayer->GetPosition());
	treasureBg	= GetTileBackground(pTreasure->GetPosition());
	exitBg		= GetTileBackground(pExit->GetPosition());

	WriteEntity(pPlayer, playerBg);
	WriteEntity(pTreasure, treasureBg);
	WriteEntity(pExit, exitBg);

	for (int i = 0; i < enemies.size(); i++)
	{
		// TODO: Needs to check if tile has a background colour to set the same background colour here
		//exists(enemies[i]->GetPosition());
		bg = GetTileBackground(enemies[i]->GetPosition());
		WriteEntity(enemies[i], bg);
	}

	for (int i = 0; i < gold.size(); i++)
	{
		// Same as above
		bg = GetTileBackground(gold[i]->GetPosition());
		WriteEntity(gold[i], bg);
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

void GameMap::RequestGoldPickup()
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

			Gold* goldCopy = gold[i]; // Copy pointer to this Gold entity

			gold.erase(gold.begin() + i); // Remove this gold piece from the vector
			
			vector<Entity*>::iterator goldEntity = find(entities.begin(), entities.end(), (Entity*)goldCopy); // Find same Gold entity in vector of all entities

			if (goldEntity != entities.end()) // If found in vector - should ALWAYS find it?
			{
				entities.erase(goldEntity); // Remove this gold piece from the vector
			}

			goldCopy->Destroy(); // Deallocate memory for this gold piece

			found = true;
		}

		i++;
	}

	pPlayer->IncrementGold(value);

	Game::DisplayText(L"Gold:  " + to_wstring(pPlayer->GetGold()), Game::goldLineNo, 6, true);
}

void GameMap::RequestEnemyKO()
{
	int		enemyIndex		= 0;
	bool	ko				= PlayerIsBehindEnemy(enemyIndex);

	if (ko && enemies[enemyIndex]->IsActive())
	{
		Game::DisplayText(L"Player knocked out an enemy!", Game::statusLineNo, 12);
		enemies[enemyIndex]->SetActive(false);
		// Then a timer should elapse in the game Run() scope (not here) for 10-20 seconds before enemy is set active again
		// ...
	}
}

bool GameMap::RequestEnemyPickpocket()
{
	int		enemyIndex = 0;
	bool	behind = PlayerIsBehindEnemy(enemyIndex);

	if (behind)
	{
		if (enemies[enemyIndex]->GetIfHasKey())
		{
			pPlayer->SetKeyObtained(true);

			Game::DisplayText(L"You found the key!", Game::statusLineNo, 13);
			Game::DisplayText(L"KEY OBTAINED", Game::progressLineNo, 22, true);
		}
		else
		{
			Game::DisplayText(L"No key here...", Game::statusLineNo, 12);
			behind = false;
		}
	}

	return (behind);
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

int GameMap::GetTileBackground(Entity::Position pos)
{
	int		bg		= 0;
	bool	found	= false;
	int		i		= 0;

	while (i < (int)tiles.size() && !found)
	{
		if (tiles[i]->GetPosition() == pos)
		{
			bg = tiles[i]->GetColour().background;
			found = true;
		}

		i++;
	}

	return (bg);
}

bool GameMap::PlayerIsBehindEnemy(int& enemyIdx)
{
	bool	found	= false;
	int		i		= 0;

	Entity::Position playerPos = pPlayer->GetPosition();
	
	// Enemy to the left: enemy should be -1 on x axis
	Entity::Position enemyLeft = playerPos;
	enemyLeft.x--;

	// Enemy to the right: enemy should be +1 on x axis
	Entity::Position enemyRight = playerPos;
	enemyRight.x++;

	// Enemy above: enemy should be -1 on y axis
	Entity::Position enemyAbove = playerPos;
	enemyAbove.y--;

	// Enemy below: enemy should be +1 on y axis
	Entity::Position enemyBelow = playerPos;
	enemyBelow.y++;

	// Check enemies clockwise - if behind multiple enemies, only take down one
	while (i < (int)enemies.size() && !found)
	{
		// Check if enemy above
		if ((enemies[i]->GetPosition() == enemyAbove) && (enemies[i]->GetDirection() == Enemy::Direction::NORTH))
		{
			enemyIdx = i;
			found = true;
		}

		// Check if enemy to the right
		else if ((enemies[i]->GetPosition() == enemyRight) && (enemies[i]->GetDirection() == Enemy::Direction::EAST))
		{
			enemyIdx = i;
			found = true;
		}

		// Check if enemy below
		else if ((enemies[i]->GetPosition() == enemyBelow) && (enemies[i]->GetDirection() == Enemy::Direction::SOUTH))
		{
			enemyIdx = i;
			found = true;
		}

		// Check if enemy to the left
		if ((enemies[i]->GetPosition() == enemyLeft) && (enemies[i]->GetDirection() == Enemy::Direction::WEST))
		{
			enemyIdx = i;
			found = true;
		}

		i++;
	}

	return (found);
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

int Game::GetColourCode(int foreground, int background)
{
	int colour = 16 * background + foreground;

	return (colour);
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
	bool newGame = StartMenu();	

	if (running)
	{
		if (newGame)
		{
			pMap->SetUpMap();
			DisplayText(L"H - Show Help", hintLineNo, 10);
			DisplayText(L"Gold:  0", goldLineNo, 6);
		}
		else
		{
			// TODO: load existing game - exit loop for now
			running = false;
		}
		
	}

	while (running)
	{
		GameLoop();
		ProcessGameInput();
		Sleep(50);
	}
	
}

/// <summary>
/// Processes user inputs during gameplay.
/// </summary>
void Game::ProcessGameInput()
{
	bool ko = false;

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
			// Prioritise checking if player is behind enemy FIRST (for pickpocketing)
			if (!pMap->RequestEnemyPickpocket())
			{
				// Otherwise just pick up gold if there's any there
				pMap->RequestGoldPickup();
			}	
			break;
		case 'f':
			pMap->RequestEnemyKO();
			break;
		default:
			break;
		}
	}
}

/// <summary>
/// Process user menu option input.
/// </summary>
/// <param name="selected">Whether a menu item was successfully selected or not</param>
/// <param name="isNewGame">Whether the user has chosen to start a brand new game or not</param>
void Game::ProcessStartupInput(bool& selected, bool& isNewGame, bool& exit)
{
	char option;

	cin >> option;

	switch (option)
	{
	case '1':
		isNewGame	= true;
		selected	= true;
		exit		= false;
		break;
	case '2':
		isNewGame	= false;
		selected	= true;
		exit		= false;
		break;
	case '3':
		selected	= true;
		exit		= true;
		break;
	default:
		selected = false;
		wcout << "\n[!] Invalid option";
		cin.clear();
		cin.ignore(10000, '\n');
		Sleep(1000);
		break;
	}
}

void Game::DisplayText(wstring text, int lineNo, int colour, bool noRewrite)
{
	static wstring last[10]; // Or however many lines max to be used 
	static int counter[10];  // " "

	int idx			= 0;	// Index of last array to place most recent string
	int realLineNo	= 0;	// Actual console line number

	if (lineNo == 0) // Lines should start at 1, assume this if 0 is passed
	{
		realLineNo = GameMap::height + 2 + lineNo + 1;
	}
	else
	{
		realLineNo = GameMap::height + 2 + lineNo;
		idx = lineNo - 1;
	}

	if (noRewrite) // User can optionally set this parameter to forcefully ignore appending a number to indicate the same message is being displayed
	{
		last[idx] = L"";
	}

	COORD coords = { 0, realLineNo };

	SetConsoleCursorPosition(handle, coords);
	SetConsoleTextAttribute(handle, colour);

	// If display text is being requested but is already identical to what's there, append an extra number to indicate this to the user
	if (text == last[idx])
	{
		counter[idx]++;

		wcout << "\r" << text << " (" << to_wstring(counter[idx] + 1) << ")                                   \n";
	}
	else if (counter[idx] > 0)
	{
		counter[idx] = 0;
		wcout << "\r" << text << "                                   \n";
	}
	else
	{
		wcout << "\r" << text << "                                   \n";
	}

	last[idx] = text;

	SetConsoleTextAttribute(handle, 7); // Reset colour
}

/// <summary>
/// Startup screen, allowing the user to start a new game (or load one?)
/// </summary>
/// <returns>Whether new game has been selected or not</returns>
bool Game::StartMenu()
{
	wifstream	startupTxt("./media/startup.txt"); // Wrap this in a try-catch in case file is deleted
	wstring		currLine;
	wstring		txt;

	bool		selected	= false;
	bool		isNewGame	= false;
	bool		quit		= false;
	bool		startNew	= false;	// Return value

	while (getline(startupTxt, currLine))
	{
		txt += currLine + L"\n";
	}

	startupTxt.close();

	while (!selected)
	{
		try
		{
			system("cls");
			wcout << txt;

			CONSOLE_SCREEN_BUFFER_INFO cbsi;
			bool worked = GetConsoleScreenBufferInfo(handle, &cbsi);

			if (!worked)
			{
				throw (worked);
			}

			COORD coords = cbsi.dwCursorPosition;

			wcout << "\n\nEnter a menu option.\t> ";

			ProcessStartupInput(selected, isNewGame, quit);
		}
		catch (bool worked)
		{
			wcout << "\n\nERROR: failed to get console cursor position";
			selected	= true;
			quit		= true;
		}
	}

	if (quit)
	{
		EndGame();
	}
	else
	{
		startNew = isNewGame;
	}

	return (startNew);
}

void Game::ShowHelp()
{
	system("cls");
	wifstream	helpTxt("./media/help.txt"); // Wrap this in a try-catch in case file is deleted
	wstring		txt;
	bool		close = false;

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
}

void Game::EndGame()
{
	// Delete map, entities etc.
	// ...
	running = false;
	system("cls");
	wcout << "Thanks for playing!\n";
}