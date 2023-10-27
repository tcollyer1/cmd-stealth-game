#include <windows.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <string>

#include "..\h\game.h"
#include "..\h\env.h"

#pragma comment(lib, "Winmm.lib") // For sound effects

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
			
			// Otherwise continue finding a position
			
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

bool GameMap::GetIfGameOver()
{
	bool				over		= false;

	Entity::Position	exitPos		= pExit->GetPosition();
	Entity::Position	playerPos	= pPlayer->GetPosition();

	if (pPlayer->GetTreasureObtained() && (exitPos == playerPos))
	{
		over = true;
	}
	else
	{
		// TODO: Else if an enemy is in full alert, game is over
		for (int i = 0; i < enemies.size(); i++)
		{
			if (enemies[i]->GetAlertLevel() == Enemy::SPOTTED)
			{
				over = true;
			}
		}
	}
	

	return (over);
}

/// <summary>
/// Plays sound effect each time a player walks on a hard/soft tile.
/// </summary>
/// <param name="t">Tile type player is walking on</param>
void GameMap::PlaySoundFX(Tile::TerrainType t)
{
	int		soundNo		= rand() % 3 + 1;
	wstring	fileName	= L"";

	switch (t)
	{
	case (Tile::HARD):
		fileName = L"./media/hardtile_" + to_wstring(soundNo) + L".wav";
		break;
	case (Tile::SOFT):
		fileName = L"./media/softtile_" + to_wstring(soundNo) + L".wav";
		break;
	default:
		break;
	}

	if (L"" != fileName)
	{
		PlaySound(fileName.c_str(), NULL, SND_FILENAME | SND_ASYNC);
	}
}

/// <summary>
/// Prepares the basic map including border walls and randomised floor tiles.
/// </summary>
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
				if (!((x == pPlayer->GetPosition().x) && (y == pPlayer->GetPosition().y)))
				{
					light = Tile::LightLevel(rand() % 3);
					terrain = Tile::TerrainType(rand() % 2);
				}
				// Always spawn player initially on a dark tile to prevent unfairness on a new game
				else
				{
					light	= Tile::DARK;
					terrain = Tile::SOFT;
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

/// <summary>
/// Redraws the current map content (all entities) back to the screen after viewing the help menu.
/// </summary>
void GameMap::RedrawMap()
{
	system("cls");

	for (int x = 0; x < entities.size(); x++)
	{
		WriteEntity(entities[x]);
	}

	Game::DisplayText(L"H - Show Help  |  E - Quit", Game::hintLineNo, Entity::GREEN, true);
	Game::DisplayText(L"Gold:  " + to_wstring(pPlayer->GetGold()), Game::goldLineNo, Entity::DARK_YELLOW, true);

	if (pPlayer->GetKeyObtained() && !pPlayer->GetTreasureObtained())
	{
		int colour = Game::GetColourCode(Entity::DARK_YELLOW, Entity::DARK_BLUE);
		Game::DisplayText(L"KEY OBTAINED", Game::progressLineNo, colour, true);
	}
	else if (pPlayer->GetTreasureObtained())
	{
		int colour = Game::GetColourCode(Entity::DARK_YELLOW, Entity::DARK_BLUE);
		Game::DisplayText(L"TREASURE OBTAINED", Game::progressLineNo, colour, true);
	}
}

/// <summary>
/// Draws the current map content to the screen on refresh.
/// </summary>
void GameMap::DrawContent()
{
	int bg, playerBg, treasureBg, exitBg = 0;

	for (int i = 0; i < gold.size(); i++)
	{
		bg = GetTileBackground(gold[i]->GetPosition());
		WriteEntity(gold[i], bg);
	}

	for (int i = 0; i < enemies.size(); i++)
	{
		bg = GetTileBackground(enemies[i]->GetPosition());
		WriteEntity(enemies[i], bg);
	}

	playerBg	= GetTileBackground(pPlayer->GetPosition());
	treasureBg	= GetTileBackground(pTreasure->GetPosition());
	exitBg		= GetTileBackground(pExit->GetPosition());

	WriteEntity(pTreasure, treasureBg);
	WriteEntity(pExit, exitBg);
	WriteEntity(pPlayer, playerBg);
}

void GameMap::UpdateEnemyAwareness(int currTimeMS)
{
	Enemy* currEnemy = NULL;

	Tile::TerrainType terrain;

	for (int i = 0; i < enemies.size(); i++)
	{
		currEnemy = enemies[i];

		terrain = pPlayer->GetCurrentTile()->GetTerrainType();

		if (terrain == Tile::HARD)
		{
			currEnemy->CheckIfInHearingRange(pPlayer->GetPosition(), currTimeMS);
		}
	}
}

/// <summary>
/// Handles a player move in a direction specified by the player.
/// </summary>
/// <param name="move">Movement direction requested</param>
/// <param name="currTimeMS">The current time elapsed - to determine when to remove enemy alertness</param>
void GameMap::RequestPlayerMove(Character::Movement move, int currTimeMS)
{
	Entity::Position oldPos = pPlayer->GetPosition();
	Entity::Position newPos = pPlayer->CalculatePos(move);

	int		i			= 0;
	bool	found		= false;

	if (GetIfTraversable(newPos, true))
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
		Tile* t = pPlayer->GetCurrentTile();
		PlaySoundFX(t->GetTerrainType());

		UpdateEnemyAwareness(currTimeMS);
	}
}

/// <summary>
/// If the player is standing on a gold piece, collects it from the map and adds to the player's total gold count.
/// </summary>
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

	Game::DisplayText(L"Gold:  " + to_wstring(pPlayer->GetGold()), Game::goldLineNo, Entity::DARK_YELLOW, true);
}

/// <summary>
/// Disables the enemy the player is behind (if any).
/// </summary>
void GameMap::RequestEnemyKO()
{
	int		enemyIndex		= 0;
	bool	ko				= PlayerIsBehindEnemy(enemyIndex);

	if (ko && enemies[enemyIndex]->IsActive())
	{
		Game::DisplayText(L"Player knocked out an enemy!", Game::statusLineNo, Entity::RED);
		enemies[enemyIndex]->SetActive(false);
		// TODO: Then a timer should elapse in the game Run() scope (not here) for 10-20 seconds before enemy is set active again?
		// ...
	}
}

bool GameMap::RequestTreasureUnlock()
{
	Entity::Position treasurePos	= pTreasure->GetPosition();
	Entity::Position playerPos		= pPlayer->GetPosition();

	// Works out if player is immediately next to the treasure (from any direction)
	// and also has the key
	bool nextTo = ((playerPos.x == treasurePos.x + 1)
					&& (playerPos.y == treasurePos.y))
					|| ((playerPos.x == treasurePos.x)
						&& (playerPos.y == treasurePos.y + 1))
					|| ((playerPos.x == treasurePos.x - 1)
						&& (playerPos.y == treasurePos.y))
					|| ((playerPos.x == treasurePos.x)
						&& (playerPos.y == treasurePos.y - 1));
	
	if (nextTo)
	{
		if (!pPlayer->GetKeyObtained())
		{
			Game::DisplayText(L"You still need the key...", Game::statusLineNo, Entity::RED);
			nextTo = false;
		}
		else
		{
			if (!pPlayer->GetTreasureObtained())
			{
				pPlayer->SetTreasureObtained(true);

				Game::DisplayText(L"You found the treasure! Find the EXIT", Game::statusLineNo, Entity::PINK);
				Game::DisplayText(L"TREASURE OBTAINED", Game::progressLineNo, 22, true);
				pPlayer->IncrementGold(100);	// Treasure will be worth 100 gold. 
												// TODO: Make fixed value?
			}
			else
			{
				Game::DisplayText(L"You already have the treasure. Find the EXIT", Game::statusLineNo, Entity::RED);
				nextTo = false;
			}
		}
	}

	return (nextTo);
}

/// <summary>
/// Pickpockets the enemy the player is behind (if any), informing the player whether the key was obtained or not.
/// </summary>
/// <returns>True if key obtained; false otherwise</returns>
bool GameMap::RequestEnemyPickpocket()
{
	int		enemyIndex = 0;
	bool	behind = PlayerIsBehindEnemy(enemyIndex);

	if (behind)
	{
		if (enemies[enemyIndex]->GetIfHasKey() && !pPlayer->GetKeyObtained())
		{
			pPlayer->SetKeyObtained(true);

			Game::DisplayText(L"You found the key!", Game::statusLineNo, Entity::PINK);

			int colour = Game::GetColourCode(Entity::DARK_YELLOW, Entity::DARK_BLUE);
			Game::DisplayText(L"KEY OBTAINED", Game::progressLineNo, colour, true); // TODO: Actual colour here
		}
		else if (pPlayer->GetKeyObtained())
		{
			Game::DisplayText(L"You already have the key. Find the treasure to unlock it.", Game::statusLineNo, Entity::RED);
			behind = false;
		}
		else
		{
			Game::DisplayText(L"No key here...", Game::statusLineNo, Entity::RED);
			behind = false;
		}
	}

	return (behind);
}

/// <summary>
/// Calculates a random move (for an enemy).
/// </summary>
/// <param name="newPos">Stores the position the the move would result in.</param>
/// <param name="move">Stores the movement type.</param>
void GameMap::CalcRandomMove(Entity::Position& newPos, Character::Movement& move)
{
	// Get random direction
	move = Character::Movement(rand() % 5);

	Entity::Position proposedPos = newPos;

	switch (move)
	{
	case Character::UP:
		proposedPos.y--;
		break;
	case Character::DOWN:
		proposedPos.y++;
		break;
	case Character::RIGHT:
		proposedPos.x++;
		break;
	case Character::LEFT:
		proposedPos.x--;
		break;
	default:
		// Default 'NOTHING' case, enemy stays where they are
		break;
	}

	if (GetIfTraversable(proposedPos))
	{
		newPos = proposedPos;
	}
}

/// <summary>
/// Prepares randomised moves for each enemy on the map.
/// </summary>
void GameMap::SetUpEnemyMoves(int currTimeMS)
{
	Enemy* currEnemy = NULL;

	Entity::Position currPos;
	Entity::Position newPos;

	// For if aware of the player
	Entity::Position targetPos;
	int xDiff, yDiff;

	Character::Movement move;

	vector<Entity::Position> usedPositions; // Holds NEW enemy positions

	for (int i = 0; i < enemies.size(); i++)
	{
		currEnemy = enemies[i];

		currEnemy->ProcessAlertedState(currTimeMS);

		if (currEnemy->IsActive()) // Don't move if not active (knocked out by player)
		{
			currPos = currEnemy->GetPosition();

			newPos	= currPos;
			move	= Character::NOTHING;

			// TODO: If enemy is in alerted state, start moving towards player
			if (currEnemy->GetAlertLevel() == Enemy::SUSPICIOUS)
			{
				targetPos = currEnemy->GetPlayerLastKnownPos();

				CalcSpecificMove(move, newPos, currPos, targetPos);
			}
			else
			{
				// Get random next move
				CalcRandomMove(newPos, move);
			}

			if ((find(usedPositions.begin(), usedPositions.end(), newPos) == usedPositions.end())) // find() points to the last element if not found
			{
				usedPositions.push_back(newPos);
				currEnemy->SetNextPos(newPos, move);
			}
			// Enemy doesn't move this turn if their first movement choice is unavailable
			else
			{
				currEnemy->SetNextPos(currPos, Character::NOTHING);
			}			
		}
	}
}

void GameMap::CalcSpecificMove(Character::Movement& move, Entity::Position& proposedPos, Entity::Position currPos, Entity::Position targetPos)
{
	int xDiff, yDiff;

	bool hasMove = false;

	xDiff = abs(currPos.x - targetPos.x);
	yDiff = abs(currPos.y - targetPos.y);

	// If the x difference is smaller than, or the same as, the y difference, try to move left/right
	// In the event the enemy cannot move in the desired direction here, attempt the y direction
	if (((xDiff < yDiff || xDiff == yDiff || yDiff == 0) && xDiff != 0))
	{
		// Try to move left/right
		if (targetPos.x < currPos.x)
		{
			proposedPos.x--;
			move = Character::LEFT;
		}
		else
		{
			proposedPos.x++;
			move = Character::RIGHT;
		}

		if (GetIfTraversable(proposedPos))
		{
			hasMove = true;
		}
		else
		{
			hasMove = false;
			proposedPos = currPos;
			move = Character::NOTHING;
		}		
	}

	// - If the y difference is smaller than the x distance, try to move up/down
	// - If the player was unable to move left/right, try to move up/down
	if ((yDiff < xDiff || ((yDiff == 0 && xDiff != 0) || yDiff != 0)) && !hasMove)
	{
		// Try to move up/down
		if (targetPos.y < currPos.y)
		{
			proposedPos.y--;
			move = Character::UP;
		}
		else
		{
			proposedPos.y++;
			move = Character::DOWN;
		}

		if (!GetIfTraversable(proposedPos))
		{
			proposedPos = currPos;
			move = Character::NOTHING;

			// Reset proposedPos and get a random move instead
			CalcRandomMove(proposedPos, move);

			if (!GetIfTraversable(proposedPos))
			{
				// If still can't move, just have enemy stay where they are
				proposedPos = currPos;
				move = Character::NOTHING;
			}
		}
	}
}

/// <summary>
/// Actions the prepared enemy moves by updating the map.
/// </summary>
void GameMap::MoveEnemies()
{
	bool found = false;

	Entity::Position oldPos;
	Entity::Position newPos;
	Enemy* currEnemy = NULL;

	int i, j;

	for (i = 0; i < enemies.size(); i++)
	{
		j		= 0;
		found	= false;

		currEnemy = enemies[i];

		oldPos = currEnemy->GetPosition();
		newPos = currEnemy->GetNextPos();

		// If enemy is going to move - must previously have been on a walkable tile, so therefore replace it with whatever existing tile was there
		if (!(oldPos == newPos) && currEnemy->IsActive()) // Check if active in the instance an enemy had a move prepared 10 iterations ago but the player has since knocked them out
		{
			while (j < tiles.size() && !found)
			{
				if (tiles[j]->GetPosition() == oldPos)
				{
					WriteEntity(tiles[j]);
					found = true;
				}

				j++;
			}

			currEnemy->Move(newPos);
		}
	}
}

/// <summary>
/// Returns whether a Player can move to a specified position or not.
/// </summary>
/// <param name="pos">Desired position</param>
/// <param name="updatePlayerTile">Flag to indicate whether to pre-prepare new player tile properties, if moving the player character</param>
/// <returns>True/false</returns>
bool GameMap::GetIfTraversable(Entity::Position pos, bool updatePlayerTile)
{
	bool traversable	= false;
	bool found			= false;
	int i = 0;

	while (i < (int)entities.size() && !found)
	{
		if (entities[i]->GetPosition() == pos)
		{
			traversable = entities[i]->GetIfPassable();

			if (traversable && updatePlayerTile)
			{
				// Update player tile to this new one
				pPlayer->UpdateCurrentTile((Tile*)entities[i]);
			}

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

/// <summary>
/// Gets if the player is currently behind an enemy.
/// </summary>
/// <param name="enemyIdx">Index of the Enemy object in the enemies vector that the player is currently behind, if any</param>
/// <returns>Whether the player is behind an enemy or not</returns>
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

/// <summary>
/// Calculates the Windows console colour code based on the requested foreground and background colour.
/// </summary>
/// <param name="foreground">Foreground colour (see Colours enum within the Entity class)</param>
/// <param name="background">Background colour</param>
/// <returns>The calculated colour code for the requested foreground/background colour combination</returns>
int Game::GetColourCode(int foreground, int background)
{
	int colour = 16 * background + foreground;

	return (colour);
}

void Game::UpdateMap()
{
	pMap->DrawContent(); // Update map
}

/// <summary>
/// Everything needed to run the game - the main entry point.
/// </summary>
void Game::Run()
{
	int timeMS	= 0;
	int iter	= 0;

	bool newGame = StartMenu();	

	if (running) // If "Quit" not selected at the start menu
	{
		if (newGame)
		{
			pMap->SetUpMap();
			DisplayText(L"H - Show Help  |  E - Quit", hintLineNo, Entity::GREEN);
			DisplayText(L"Gold:  0", goldLineNo, Entity::DARK_YELLOW);
		}
		else
		{
			// TODO: load existing game - exit loop for now
			running = false;
		}
		
	}

	while (running)
	{
		iter++;
		UpdateMap();
		ProcessGameInput(timeMS);

		// Process if player is at exit here
		if (pMap->GetIfGameOver())
		{
			EndGame();
		}
		else
		{
			if ((iter % 50 == 0) && (iter % 100 != 0))
			{
				// Every 50 game cycles, prepare enemies' next moves and rotate their position accordingly
				pMap->SetUpEnemyMoves(timeMS);
			}
			else if ((iter % 100 == 0))
			{
				// Every other 50 game cycles, action next enemy moves
				pMap->MoveEnemies();
			}

			Sleep(10);		// 10ms delay between map redraws
			timeMS += 10;	// Increase timer by +10ms
		}
	}	
}

/// <summary>
/// Processes user inputs during gameplay.
/// </summary>
void Game::ProcessGameInput(int currTimeMS)
{
	bool ko = false;

	if (_kbhit())
	{
		switch (_getch())
		{
		case 'w':
			pMap->RequestPlayerMove(Character::UP, currTimeMS);
			break;
		case 's':
			pMap->RequestPlayerMove(Character::DOWN, currTimeMS);
			break;
		case 'a':
			pMap->RequestPlayerMove(Character::LEFT, currTimeMS);
			break;
		case 'd':
			pMap->RequestPlayerMove(Character::RIGHT, currTimeMS);
			break;
		case 'e':
			EndGame();
			break;
		case 'h':
			ShowHelp();
			break;
		case 32: // Space
			// Prioritise checking if player is behind enemy FIRST (for pickpocketing), followed by unlocking the treasure
			if (!pMap->RequestEnemyPickpocket())
			{
				if (!pMap->RequestTreasureUnlock())
				{
					// Otherwise just request to pick up gold if there's any there
					pMap->RequestGoldPickup();
				}
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
	wstring option;

	wcin >> option;

	if (option == L"1")
	{
		isNewGame	= true;
		selected	= true;
		exit		= false;
	}
	else if (option == L"2")
	{
		isNewGame	= false;
		selected	= true;
		exit		= false;
	}
	else if (option == L"3")
	{
		selected	= true;
		exit		= true;
	}
	else
	{
		selected	= false;
		wcout << "\n[!] Invalid option";
		cin.clear();
		cin.ignore(10000, '\n');
		Sleep(1000);
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

	COORD coords = { 0, (short)realLineNo };

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
	bool		worked		= false;

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
			worked = GetConsoleScreenBufferInfo(handle, &cbsi);

			if (!worked)
			{
				throw (worked);
			}

			COORD coords = cbsi.dwCursorPosition;

			wcout << "\n\nEnter a menu option.\n> ";

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