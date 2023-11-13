#include <windows.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <string>

#include "..\h\game.h"
#include "..\h\env.h"
#include "..\h\wall.h"

#pragma comment(lib, "Winmm.lib") // For sound effects

static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); // Global for standard reuse

/// <summary>
/// Safely destroys all entities on the map and frees up all used memory.
/// </summary>
void GameMap::DestroyEverything()
{
	int entitiesSize = (int)entities.size();

	for (int i = 0; i < entitiesSize; i++)
	{
		entities[i]->Destroy();
	}

	tiles.clear();
	enemies.clear();
	walls.clear();
	gold.clear();
	wallBlocks.clear();
	entities.clear();
}

/// <summary>
/// Adds the essential entities to the map - including the player, treasure, exit and randomised walls.
/// </summary>
void GameMap::SetEssentialEntities()
{
	/* Generate random in-game walls,
	 * per quarter of the map. Either 0 walls, 1x large + up to 2x small, or up to 4x small
	 */
	AddRandomWalls(topLeft.x, topLeft.y);
	AddRandomWalls(topRight.x, topRight.y);
	AddRandomWalls(btmLeft.x, btmLeft.y);
	AddRandomWalls(btmRight.x, btmRight.y);

	pPlayer		= AddEntity<Player>(pPlayer);
	pTreasure	= AddEntity<Treasure>(pTreasure);
	pExit		= AddEntity<Exit>(pExit);

	Enemy* pE	= NULL;
	pE			= AddEntity<Enemy>(pE, true);
	this->enemies.push_back(pE);
}

GameMap::GameMap(int enemies, int gold)
{
	topLeft.x = 1;
	topLeft.y = 1;

	topRight.x = (width / 2) + 1; // Offset by 1
	topRight.y = 1;

	btmLeft.x = 1;
	btmLeft.y = (height / 2) + 1;

	btmRight.x = (width / 2) + 1;
	btmRight.y = (height / 2) + 1;

	numEnemies	= enemies;
	goldAmount	= gold;

	SetEssentialEntities();

	// Populate map with enemies and gold
	AddEntities<Enemy>(numEnemies - 1, this->enemies); // -1 for the one enemy already on the map
	AddEntities<Gold>(goldAmount, this->gold);
}

/// <summary>
/// Template function that adds a single entity to the map at a randomised position.
/// </summary>
/// <typeparam name="T">Entity type to add</typeparam>
/// <param name="pEntity">Pointer to the entity to add for storage</param>
/// <param name="optionalFlag">Optional flag required for some entities (i.e. indicating an enemy with a key)</param>
/// <returns>Pointer to the entity created</returns>
template<typename T>
T* GameMap::AddEntity(T* pEntity, bool optionalFlag)
{
	bool				positionAdded;
	Entity::Position	tempPos;
	int					y, x;

	positionAdded = false;

	while (!positionAdded)
	{
		y = (rand() % height) + 1;
		x = (rand() % width) + 1;

		tempPos.x = x;
		tempPos.y = y;

		if (find_if(entities.begin(), entities.end(), [&tempPos](Entity* ent) {return ent->GetPosition() == tempPos; }) == entities.end()) // find() points to the last element if not found
		{
			pEntity = new T(x, y, optionalFlag);
	
			entities.push_back(pEntity);

			positionAdded = true;
		}

		// Otherwise continue finding a position
	}

	return (pEntity);
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

	// Add all positions currently in entities vector to usedPositions so they cannot be reused
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

/// <summary>
/// Gets whether the game has ended, either due to enemy detection or the player successfully completing the level.
/// </summary>
/// <returns>True if parameters for a game ending have been fulfilled</returns>
bool GameMap::GetIfGameOver(int timeMS, int& score)
{
	bool	over	= false;

			score	= 0;
	int		temp	= 0;
	int		gold	= pPlayer->GetGold();

	Entity::Position	exitPos		= pExit->GetPosition();
	Entity::Position	playerPos	= pPlayer->GetPosition();

	if (pPlayer->GetTreasureObtained() && (exitPos == playerPos))
	{
		temp = Game::multiplier - timeMS;
		temp = (temp < 0 ? 0 : temp);
		score = temp + (1 * gold);
		over = true;
	}
	else
	{
		for (int i = 0; i < enemies.size(); i++)
		{
			if (enemies[i]->GetAlertLevel() == Enemy::SPOTTED)
			{
				Game::DisplayText(L"GAME OVER...", Game::statusLineNo, Entity::DARK_RED, true);
				over = true;
			}
		}
	}
	

	return (over);
}

/// <summary>
/// This function is called to play sound effect each time a player walks on a hard/soft tile.
/// </summary>
/// <param name="t">Tile terrain type player is walking on (hard/soft)</param>
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

	if (Game::GetIfFileExists(fileName))
	{
		PlaySound(fileName.c_str(), NULL, SND_FILENAME | SND_ASYNC);
	}
}

/// <summary>
/// Checks if a wall block is already here/directly next to the proposed wall block.
/// </summary>
/// <param name="wb">Proposed WallBlock object to check</param>
/// <returns>True if a wall block is too close/would directly collide</returns>
bool GameMap::GetIfWallHere(WallBlock wb)
{
	bool	isWall		= false;
	int		i			= 0;
	int		j			= 0;
	int		k			= 0;

	int		leftPos		= 0;
	int		rightPos	= 0;

	if (wb.GetOrientation() == WallBlock::HORIZONTAL)
	{
		leftPos		= wb.GetPosition().y - 1;
		rightPos	= wb.GetPosition().y + 1;
	}
	else
	{
		leftPos		= wb.GetPosition().x - 1;
		rightPos	= wb.GetPosition().x + 1;
	}

	while (i < wallBlocks.size() && !isWall)
	{
		j = 0;

		// If this wall block has the same orientation, check it
		if (wallBlocks[i]->GetOrientation() == wb.GetOrientation())
		{
			while (j < wb.GetLength() && !isWall)
			{
				k = 0;

				// Check none of the proposed blocks in this wall block surround an existing wall
				if ((wallBlocks[i]->GetIfAdjacent(wb.GetWallBlock(j)->GetPosition()))
					|| (wallBlocks[i]->GetIfAtEdge(wb.GetWallBlock(j)->GetPosition())))
				{
					isWall = true;
				}
				else
				{
					while (k < wallBlocks[i]->GetLength() && !isWall)
					{
						// Check none of the wall blocks from either wall intersect with each other
						if ((wb.GetWallBlock(j)->GetPosition() == wallBlocks[i]->GetWallBlock(k)->GetPosition()))
						{
							isWall = true;
						}

						k++;
					}
				}				

				j++;
			}
		}

		i++;
	}

	return (isWall);
}

/// <summary>
/// Generates a random combination of wall structures on one portion of the map.
/// </summary>
/// <param name="cornerX">The top left X position of the partition of map space to generate walls for</param>
/// <param name="cornerY">The top left Y position of the patition of map space to generate walls for</param>
void GameMap::AddRandomWalls(int cornerX, int cornerY)
{
	WallBlock::Size			wallSize;
	WallBlock::Orientation	wallOri;
	int						startX, startY, maxX, maxY;
	WallBlock*				wallB = NULL;

	bool	largeWallPlaced		= false;
	bool	smallWallPlaced		= false;

	Entity::Position tempPos;

	wallSize = WallBlock::Size(rand() % 2);

	if (wallSize == WallBlock::LARGE)
	{
		// Generate one large wall
		wallOri = WallBlock::Orientation(rand() % 2);

		if (wallOri == WallBlock::HORIZONTAL)
		{
			// Generate position for large horizontal wall; offset by the wallGap (2) so wall is not placed directly next to another wall
			maxY	= ((cornerY - 1) + (height / 2)) - WallBlock::wallGap;
			startY	= rand() % (maxY - (cornerY + WallBlock::wallGap)) + (cornerY + WallBlock::wallGap);

			// If not the left side of the map (horizontal walls are drawn left -> right)
			if (cornerX != 1)
			{
				// Align to right side instead
				startX = (width + 1) - WallBlock::GetWallBlockSize(wallSize, wallOri);	// +1 to include coordinate it's currently on -
																						// e.g. 50 - 3 gives 47 but placing a block at x pos
																						// 17 places walls at x = 47, 48 & 49 but not 50
			}
			else
			{
				// Wall already left-aligned
				startX = cornerX;
			}				
		}
		else
		{
			// Generate position for vertical wall
			// If not the top side of the map (vertical walls are drawn top -> bottom)
			if (cornerY != 1)
			{
				startY = (height + 1) - WallBlock::GetWallBlockSize(wallSize, wallOri);
			}
			else
			{
				startY = cornerY;
			}
				
			maxX	= ((cornerX - 1) + (width / 2)) - WallBlock::wallGap;
			startX	= rand() % (maxX - (cornerX + WallBlock::wallGap)) + (cornerX + WallBlock::wallGap);
		}

		wallB = new WallBlock(startX, startY, wallOri, wallSize);

		for (int i = 0; i < wallB->GetLength(); i++)
		{
			Wall* pWall = wallB->GetWallBlock(i);

			walls.push_back(pWall);
			entities.push_back(pWall);
		}

		// Push wall block to wallBlocks vector
		wallBlocks.push_back(wallB);

		largeWallPlaced = true;
	}

	int numWalls = 0;

	if (largeWallPlaced)
	{
		// If a large wall has been placed, place up to 2 additional small ones
		numWalls = rand() % 1 + 1;
	}
	else
	{
		// Otherwise place up to 4 small walls
		numWalls = rand() % 3 + 1;
	}	

	for (int i = 0; i < numWalls; i++)
	{
		wallSize		= WallBlock::SMALL;
		startX			= 0;
		startY			= 0;
		smallWallPlaced = false;
		wallB			= NULL;

		wallOri = WallBlock::Orientation(rand() % 2);
			
		// Same process for small horizontal & vertical walls
		while (!smallWallPlaced)
		{
			maxY = ((cornerY - 1)  + (height / 2) - WallBlock::wallGap);
			maxX = ((cornerX - 1) + (width / 2) - WallBlock::wallGap);

			// Decrease length of block off of available space to place wall
			if (wallOri == WallBlock::HORIZONTAL)
			{
				maxX = (maxX + 1) - WallBlock::GetWallBlockSize(wallSize, wallOri);
			}
			else
			{
				maxY = (maxY + 1) - WallBlock::GetWallBlockSize(wallSize, wallOri);
			}

			startY = rand() % (maxY - (cornerY + WallBlock::wallGap)) + (cornerY + WallBlock::wallGap);
			startX = rand() % (maxX - (cornerX + WallBlock::wallGap)) + (cornerX + WallBlock::wallGap);

			tempPos.x = startX;
			tempPos.y = startY;

			wallB = new WallBlock(startX, startY, wallOri, wallSize);

			if (!GetIfWallHere(*wallB))
			{
				smallWallPlaced = true;
			}
		}			

		//wallB = new WallBlock(startX, startY, wallOri, wallSize);

		if (wallB != NULL)
		{
			for (int i = 0; i < wallB->GetLength(); i++)
			{
				Wall* pWall = wallB->GetWallBlock(i);

				walls.push_back(pWall);
				entities.push_back(pWall);
			}

			// Push wall block to wallBlocks vector
			wallBlocks.push_back(wallB);
		}
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

	/* Set up map border walls */
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
		for (x = 0; x <= width + 1; x++)
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
					light	= Tile::LightLevel(rand() % 3);
					terrain = Tile::TerrainType(rand() % 2);

					Tile* tile = new Tile(x, y, terrain, light); // Generate a random tile

					WriteEntity(tile);
					tiles.push_back(tile);
					entities.push_back(tile);
				}
				// Always spawn player initially on a dark tile to prevent unfairness on a new game
				else
				{
					light	= Tile::DARK;
					terrain = Tile::SOFT;

					Tile* tile = new Tile(x, y, terrain, light);

					pPlayer->UpdateCurrentTile(tile);

					WriteEntity(tile);
					tiles.push_back(tile);
					entities.push_back(tile);
				}
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

	for (int i = 0; i < wallBlocks.size(); i++)
	{
		for (int w = 0; w < wallBlocks[i]->GetLength(); w++)
		{
			WriteEntity(wallBlocks[i]->GetWallBlock(w));
		}
	}
}

/// <summary>
/// Handles the display of the player detection meter.
/// </summary>
void GameMap::OutputDetectionStr()
{
	int				currDetection	= 0;
	int				detection		= 0;
	wstring			str;
	Entity::Colours colour = Entity::WHITE;

	for (int x = 0; x < enemies.size(); x++)
	{
		currDetection = enemies[x]->GetDetectionLevel();
		if (currDetection > detection)
		{
			detection = currDetection;
		}
	}

	switch (detection)
	{
	case 0:
		str = L"Detection:  [......]";
		colour = Entity::GREEN;
		break;
	case 1:
		str = L"Detection:  [|.....]";
		colour = Entity::YELLOW;
		break;
	case 2:
		str = L"Detection:  [||....]";
		colour = Entity::DARK_YELLOW;
		break;
	case 3:
		str = L"Detection:  [|||...]";
		colour = Entity::RED;
		break;
	case 4:
		str = L"Detection:  [||||..]";
		colour = Entity::RED;
		break;
	case 5:
		str = L"Detection:  [|||||.]";
		colour = Entity::DARK_RED;
		break;
	case 6:
		str = L"Detection:  [||||||]";
		colour = Entity::DARK_RED;
		break;
	}

	Game::DisplayText(str, Game::alertnessLineNo, colour, true);
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

	Game::DisplayText(L"H - Show Help  |  E - Quit", Game::hintLineNo, Entity::WHITE, true);
	Game::DisplayText(L"Gold:  " + to_wstring(pPlayer->GetGold()), Game::goldLineNo, Entity::DARK_YELLOW, true);
	OutputDetectionStr();

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

	// Redraw wall blocks to be on top
	for (int i = 0; i < wallBlocks.size(); i++)
	{
		for (int w = 0; w < wallBlocks[i]->GetLength(); w++)
		{
			WriteEntity(wallBlocks[i]->GetWallBlock(w));
		}
	}
}

/// <summary>
/// Draws the current map content to the screen on refresh.
/// </summary>
void GameMap::DrawContent()
{
	int bg, playerBg, treasureBg, exitBg = 0;

	UpdateTreasureDisplay();


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

	OutputDetectionStr();
}

/// <summary>
/// Gets if the player is currently behind a wall that, presuming the player is in an enemy's line of sight, would block this.
/// </summary>
/// <param name="currPos">Current enemy position - to relate to player/wall coordinates</param>
/// <param name="dir">Current direction enemy is facing</param>
/// <returns>True if an enemy's line of sight would be blocked by a wall</returns>
bool GameMap::IsPlayerBehindWall(Entity::Position currPos, Enemy::Direction dir)
{
	Entity::Position playerPos = pPlayer->GetPosition();
	Entity::Position currWallPos;

	bool obstructs = false;

	switch (dir)
	{
	case (Enemy::NORTH):
		for (int i = 0; i < walls.size(); i++)
		{
			currWallPos = walls[i]->GetPosition();

			if (currWallPos.x == playerPos.x)
			{
				if ((currWallPos.y > playerPos.y && currWallPos.y < currPos.y) && (currPos.x == playerPos.x
					|| ((currPos.x == playerPos.x + 1 || currPos.x == playerPos.x - 1) && currWallPos.y == playerPos.y + 1)))
				{
					obstructs = true;
				}
			}
		}
		break;
	case (Enemy::SOUTH):
		for (int i = 0; i < walls.size(); i++)
		{
			currWallPos = walls[i]->GetPosition();

			if (currWallPos.x == playerPos.x)
			{
				if ((currWallPos.y < playerPos.y && currWallPos.y > currPos.y) && (currPos.x == playerPos.x
					|| ((currPos.x == playerPos.x + 1 || currPos.x == playerPos.x - 1) && currWallPos.y == playerPos.y - 1)))
				{
					obstructs = true;
				}
			}
		}
		break;
	case (Enemy::EAST):
		for (int i = 0; i < walls.size(); i++)
		{
			currWallPos = walls[i]->GetPosition();

			if (currWallPos.y == playerPos.y)
			{
				if ((currWallPos.x < playerPos.x && currWallPos.x > currPos.x) && (currPos.y == playerPos.y
					|| ((currPos.y == playerPos.y + 1 || currPos.y == playerPos.y - 1) && currWallPos.x == playerPos.x - 1)))
				{
					obstructs = true;
				}
			}
		}
		break;
	case (Enemy::WEST):
		for (int i = 0; i < walls.size(); i++)
		{
			currWallPos = walls[i]->GetPosition();

			if (currWallPos.y == playerPos.y)
			{
				if ((currWallPos.x > playerPos.x && currWallPos.x < currPos.x) && (currPos.y == playerPos.y
					|| ((currPos.y == playerPos.y + 1 || currPos.y == playerPos.y - 1) && currWallPos.x == playerPos.x + 1)))
				{
					obstructs = true;
				}
			}
		}
		break;
	}

	return (obstructs);
}

/// <summary>
/// Checks for the Player being in any of the enemies' hearing range/line of sight while alerted to update alert levels.
/// </summary>
/// <param name="currTimeMS">Current elapsed time in ms</param>
void GameMap::UpdateEnemyAwareness(int currTimeMS)
{
	Enemy* currEnemy = NULL;

	Tile::TerrainType	terrain;
	Tile::LightLevel	lightLevel;

	bool checkHearingThisTurn = false;

	static Entity::Position playerPos;

	// If player hasn't moved, don't update as if they've moved on to a different hard tile and caused more noise
	if (playerPos == pPlayer->GetPosition())
	{
		checkHearingThisTurn = false;
	}
	else
	{
		checkHearingThisTurn = true;
		playerPos = pPlayer->GetPosition();
	}

	for (int i = 0; i < enemies.size(); i++)
	{
		currEnemy = enemies[i];

		terrain		= pPlayer->GetCurrentTile()->GetTerrainType();
		lightLevel	= pPlayer->GetCurrentTile()->GetLightLevel();

		if (terrain == Tile::HARD && checkHearingThisTurn)
		{
			currEnemy->CheckIfInHearingRange(pPlayer->GetPosition(), currTimeMS);
		}

		if (!IsPlayerBehindWall(currEnemy->GetPosition(), currEnemy->GetDirection()))
		{
			currEnemy->CheckIfInLOS(pPlayer->GetPosition(), currTimeMS, lightLevel);
		}		
	}
}

/// <summary>
/// Handles a player move in a direction specified by the player.
/// </summary>
/// <param name="move">Movement direction requested</param>
/// <param name="currTimeMS">The current time elapsed - to determine when to remove enemy alertness</param>
void GameMap::RequestPlayerMove(Character::Movement move)
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
void GameMap::RequestEnemyKO(int timeMS)
{
	int		enemyIndex		= 0;
	bool	ko				= PlayerIsBehindEnemy(enemyIndex);

	if (ko && enemies[enemyIndex]->IsActive())
	{
		Game::DisplayText(L"Player knocked out an enemy!", Game::statusLineNo, Entity::RED);
		enemies[enemyIndex]->SetActive(false, timeMS);
		enemies[enemyIndex]->ClearDetectionLevel();
		enemies[enemyIndex]->ResetIntermediatePos();
	}
}

/// <summary>
/// Calculates if the player is stood next to the treasure or not.
/// </summary>
/// <returns>True if player is directly north/east/south/west of the treasure</returns>
bool GameMap::PlayerIsNextToTreasure()
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

	return (nextTo);
}

/// <summary>
/// Gets whether to request that the Treasure is displayed on the map or not.
/// </summary>
void GameMap::UpdateTreasureDisplay()
{
	if (PlayerIsNextToTreasure())
	{
		// Mark treasure as found
		pTreasure->MarkAsFound(true);
	}
}

/// <summary>
/// Handles the Player unlocking the Treasure.
/// </summary>
/// <returns>True if the Treasure was able to be unlocked, false if not</returns>
bool GameMap::RequestTreasureUnlock()
{
	Entity::Position treasurePos	= pTreasure->GetPosition();
	Entity::Position playerPos		= pPlayer->GetPosition();

	bool nextTo = PlayerIsNextToTreasure();
	
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
				pPlayer->IncrementGold(Game::treasureReward);	// Treasure will be worth 100 gold. 

				Game::DisplayText(L"Gold:  " + to_wstring(pPlayer->GetGold()), Game::goldLineNo, Entity::DARK_YELLOW, true);
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
			Game::DisplayText(L"KEY OBTAINED", Game::progressLineNo, colour, true);
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
/// <param name="c">Pointer to the Enemy to calculate a move for.</param>
/// <param name="newPos">Stores the position the the move would result in.</param>
/// <param name="move">Stores the movement type.</param>
void GameMap::CalcRandomMove(Enemy* c, Entity::Position& newPos, Character::Movement& move)
{
	// Get random direction
	move = Character::Movement(rand() % 5);

	Entity::Position proposedPos = newPos;

	proposedPos = c->CalculatePos(move);

	if (GetIfTraversable(proposedPos))
	{
		newPos = proposedPos;
	}
}

/// <summary>
/// Returns the wall block, if any, a position is associated with.
/// </summary>
/// <param name="pos">Position to check if part of a wall block</param>
/// <returns>Null if there is no wall block; otherwise a pointer to the relevant wall block with a wall at this position</returns>
WallBlock* GameMap::GetWallBlock(Entity::Position pos)
{
	bool wbFound = false;
	int i = 0;
	int j = 0;

	WallBlock* wb = NULL;
	while (i < wallBlocks.size() && !wbFound)
	{
		j = 0;
		while (j < wallBlocks[i]->GetLength() && !wbFound)
		{
			if (wallBlocks[i]->GetWallBlock(j)->GetPosition() == pos)
			{
				wb = wallBlocks[i];
				wbFound = true;
			}

			j++;
		}

		i++;
	}

	return (wb);
}

/// <summary>
/// Calculates a move with a specific goal (enemy navigating to last known player location).
/// </summary>
/// <param name="c">Pointer to the Enemy to calculate a move for.</param>
/// <param name="move">Stores the movement type - assumed by default as NOTHING.</param>
/// <param name="proposedPos">Stores the proposed coordinates to move to - when passed in, is assumed to match currPos.</param>
/// <param name="currPos">Enemy's current position.</param>
/// <param name="targetPos">Enemy's end target position</param>
/// <param name="reroute">
/// Indicates whether the algorithm has determined an enemy's desired move is obstructed by a wall - so reroute target position to 
///	the navigate the wall.
/// </param>
/// <param name="recurse">Used in case y axis move was desired first and was unable to move, so try an x axis move instead</param>
void GameMap::CalcSpecificMove(Enemy* c, Character::Movement& move, Entity::Position& proposedPos, Entity::Position currPos, Entity::Position& targetPos, bool& reroute, bool recurse)
{
	int		xDiff, yDiff;

	bool	hasMove		= false;

	int i = 0;
	int j = 0;

	WallBlock* wb = NULL;

	xDiff = abs(currPos.x - targetPos.x);
	yDiff = abs(currPos.y - targetPos.y);

	if (xDiff == 0 && yDiff == 0)
	{
		hasMove = true;
	}

	// If the x difference is smaller than, or the same as, the y difference, try to move left/right
	// In the event the enemy cannot move in the desired direction here, attempt the y direction
	if ((((xDiff < yDiff || xDiff == yDiff || yDiff == 0) && xDiff != 0) || (recurse)))
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
			// Failed to move along x axis, check if due to wall
			wb = GetWallBlock(proposedPos);

			// If there is a wall obstructing, and it's not at the edge (if at the edge, just move normally by the y axis instead),
			// check whether to prioritise moving up or down
			if (wb != NULL && !wb->GetIfAtEdge(proposedPos) && !reroute)
			{
				Entity::Position interPos;

				int wallTopDiff = abs(wb->GetPosition().y - proposedPos.y);
				int wallBtmDiff = abs(wb->GetEndPosition().y - proposedPos.y);

				if (wallTopDiff < wallBtmDiff)
				{
					interPos.x = wb->GetPosition().x;
					interPos.y = wb->GetPosition().y - 1;
				}
				else
				{
					interPos.x = wb->GetEndPosition().x;
					interPos.y = wb->GetEndPosition().y + 1;
				}

				reroute = true;

				proposedPos = currPos;
				move = Character::NOTHING;

				CalcSpecificMove(c, move, proposedPos, currPos, interPos, reroute);
				targetPos = interPos;
				hasMove = true;
			}
			// Else just try the y axis
			else
			{
				hasMove = false;
				proposedPos = currPos;
				move = Character::NOTHING;
			}
		}		
	}

	// - If the y difference is smaller than the x distance, try to move up/down
	// - If unable to move left/right, try to move up/down
	if ((yDiff < xDiff || ((yDiff == 0 && xDiff != 0) || yDiff != 0)) && !hasMove)
	{
		// Try to move up/down
		if (targetPos.y < currPos.y && !recurse)
		{
			proposedPos.y--;
			move = Character::UP;
		}
		else if (!recurse)
		{
			proposedPos.y++;
			move = Character::DOWN;
		}
		else
		{
			// Can't move by desired x or y pos, so attempt to move randomly
			proposedPos = currPos;
			move = Character::NOTHING;

			CalcRandomMove(c, proposedPos, move);

			if (!GetIfTraversable(proposedPos))
			{
				// If still can't move, just have enemy stay where they are
				proposedPos = currPos;
				move = Character::NOTHING;
			}
		}

		if (!GetIfTraversable(proposedPos) && !recurse)
		{
			// Failed to move along y axis, check if due to wall
			wb = GetWallBlock(proposedPos);

			// If there is a wall obstructing, and it's not at the edge (if at the edge, just move normally by the y axis instead),
			// check whether to prioritise moving up or down
			if (wb != NULL && !wb->GetIfAtEdge(proposedPos) && !reroute)
			{
				Entity::Position interPos;

				int wallLeftDiff = abs(wb->GetPosition().x - proposedPos.x);
				int wallRightDiff = abs(wb->GetEndPosition().x - proposedPos.x);

				if (wallLeftDiff < wallRightDiff)
				{
					interPos.x = wb->GetPosition().x - 1;
					interPos.y = wb->GetPosition().y;
				}
				else
				{
					interPos.x = wb->GetEndPosition().x + 1;
					interPos.y = wb->GetEndPosition().y;
				}

				reroute = true;

				proposedPos = currPos;
				move = Character::NOTHING;

				CalcSpecificMove(c, move, proposedPos, currPos, interPos, reroute);
				targetPos = interPos;
				hasMove = true;


			}
			else
			{
				proposedPos = currPos;
				move = Character::NOTHING;

				// Check x move
				CalcSpecificMove(c, move, proposedPos, currPos, targetPos, reroute, true);
			}
		}
	}
}

/// <summary>
/// Prepares randomised moves for each enemy on the map.
/// </summary>
/// <param name="currTimeMS">Current elapsed game time</param>
void GameMap::SetUpEnemyMoves(int currTimeMS)
{
	Enemy* currEnemy = NULL;
	bool reroute = false;

	Entity::Position currPos;
	Entity::Position newPos;

	// For if aware of the player
	Entity::Position targetPos;

	Character::Movement move;

	vector<Entity::Position> usedPositions; // Holds NEW enemy positions

	for (int i = 0; i < enemies.size(); i++)
	{
		currEnemy = enemies[i];

		if (currEnemy->IsActive()) // Don't move if not active (knocked out by player)
		{
			currPos = currEnemy->GetPosition();

			newPos	= currPos;
			move	= Character::NOTHING;

			currEnemy->ProcessAlertedState(currTimeMS, pPlayer->GetPosition());

			if (currEnemy->GetAlertLevel() == Enemy::SUSPICIOUS)
			{
				targetPos = currEnemy->GetPlayerLastKnownPos();

				// If navigating a wall, set target position to the intermediate position
				currEnemy->GetIntermediatePos(targetPos);

				CalcSpecificMove(currEnemy, move, newPos, currPos, targetPos, reroute);

				if (reroute)
				{
					reroute = false;
					currEnemy->SetIntermediatePos(targetPos);
				}

				Entity::Position interPos;
				currEnemy->GetIntermediatePos(interPos);

				if (newPos == interPos)
				{
					currEnemy->ResetIntermediatePos();
				}
			}
			else
			{
				// Get random next move
				CalcRandomMove(currEnemy, newPos, move);
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
		else
		{
			currEnemy->ProcessKOState(currTimeMS);
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

/// <summary>
/// Gets the colour of the tile's background in the instance an entity is temporarily overwriting it
/// (e.g. player/enemy walking over different tiles) so as not to change it
/// </summary>
/// <param name="pos">Position of the tile in question</param>
/// <returns>Background colour of the tile at the specified position</returns>
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

/// <summary>
/// Initialise Game object.
/// </summary>
Game::Game()
{
	// Make the console cursor invisible
	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(handle, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(handle, &cursorInfo);

	pMap	= NULL;
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

	gameOpen = true;

	while (gameOpen)
	{
		score	= 0;
		running = true;
		timeMS	= 0;
		iter	= 0;

		bool newGame = StartMenu();

		if (running) // If "Quit" not selected at the start menu
		{
			if (newGame)
			{
				pMap = new GameMap(numEnemies, goldAmount);
				pMap->SetUpMap();
				DisplayText(L"H - Show Help  |  E - Quit", hintLineNo, Entity::WHITE, true);
				DisplayText(L"Gold:  0", goldLineNo, Entity::DARK_YELLOW, true);
				DisplayText(L"Detection:  [.....]", alertnessLineNo, Entity::GREEN, true);
			}
			else
			{
				running = false;
			}

		}

		while (running)
		{
			iter++;
			UpdateMap();
			ProcessGameInput(timeMS);

			DisplayText(L"Elapsed time:  " + to_wstring(timeMS) + L"ms", timerLineNo, Entity::CYAN, true);

			// Process if player is at exit here
			if (running)
			{
				if (pMap->GetIfGameOver(timeMS, score))
				{
					Sleep(3000);
					GameOver();
				}
				else
				{
					if ((iter % 30 == 0) && (iter % 60 != 0))
					{
						pMap->UpdateEnemyAwareness(timeMS);
						// Every 30 game cycles, prepare enemies' next moves and rotate their position accordingly (was every 50)
						pMap->SetUpEnemyMoves(timeMS);
					}
					else if ((iter % 60 == 0))
					{
						// Every other 30 game cycles, action next enemy moves
						pMap->MoveEnemies();
					}

					Sleep(10);		// 10ms delay between map redraws
					timeMS += 10;	// Increase timer by +10ms
				}
			}
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
			pMap->DestroyEverything();
			running		= false;
			gameOpen	= false;
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
			pMap->RequestEnemyKO(currTimeMS);
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

/// <summary>
/// Function that displays text to the screen at a specified line.
/// </summary>
/// <param name="text">Text to display</param>
/// <param name="lineNo">Line number to display text at</param>
/// <param name="colour">Colour of the text</param>
/// <param name="noRewrite">Optional flag to forcibly indicate that this message is not the same as the previous message on this line</param>
void Game::DisplayText(wstring text, int lineNo, int colour, bool noRewrite)
{
	static wstring last[10];
	static int counter[10]; 

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
	wstring		currLine;
	wstring		txt;

	bool		selected	= false;
	bool		isNewGame	= false;
	bool		quit		= false;
	bool		startNew	= false;	// Return value
	bool		worked		= false;	

	// Backup title in case banner file has been deleted/moved
	if (!Game::GetIfFileExists(L"./media/banner.txt"))
	{
		txt = L"-- CMD Stealth --";
	}
	else
	{
		wifstream startupTxt("./media/banner.txt");

		while (getline(startupTxt, currLine))
		{
			txt += currLine + L"\n";
		}

		startupTxt.close();
	}

	txt += L"\n\n  1.\tNew Game\n  2.\tQuit\n";

	while (!selected)
	{
		try
		{
			system("cls");
			wcout << txt;

			CONSOLE_SCREEN_BUFFER_INFO csbi;
			worked = GetConsoleScreenBufferInfo(handle, &csbi);

			if (!worked)
			{
				throw (worked);
			}

			COORD coords = csbi.dwCursorPosition;

			wcout << "\n\nEnter a menu option.\n> ";

			ProcessStartupInput(selected, isNewGame, quit);
		}
		catch (bool worked)
		{
			wcout << "\n\nERROR: failed to get console cursor position (" << worked << ")";
			selected	= true;
			quit		= true;
		}
	}

	if (quit)
	{
		pMap->DestroyEverything();
		running		= false;
		gameOpen	= false;
	}
	else
	{
		startNew = isNewGame;
	}

	return (startNew);
}

/// <summary>
/// Displays a help screen detailing how the game works, symbol meanings and controls.
/// </summary>
void Game::ShowHelp()
{
	system("cls");

	wstring		txt;
	wstring		currLine;
	bool		close = false;

	if (!Game::GetIfFileExists(L"./media/help.txt"))
	{
		wcout << "ERROR: help.txt has been moved, renamed or deleted.\n";
	}
	else
	{
		wifstream helpTxt("./media/help.txt");

		while (getline(helpTxt, currLine))
		{
			txt += currLine + L"\n";
		}

		helpTxt.close();

		wcout << txt;
	}	

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

/// <summary>
/// Handles the end of the game, either closing the program or restarting.
/// </summary>
void Game::GameOver()
{
	bool selected = false;

	pMap->DestroyEverything();

	wstring		userInput;
	wstring		currLine;
	int			highScore	= 0;

	// Backup title in case banner file has been deleted/moved
	if (!Game::GetIfFileExists(L"./media/score.txt"))
	{
		wofstream scoreOutp("./media/score.txt");
		scoreOutp << score;
		scoreOutp.close();
	}
	else
	{
		wifstream scoreTxt("./media/score.txt");

		getline(scoreTxt, currLine);
		highScore = stoi(currLine);

		scoreTxt.close();
	}


	while (!selected)
	{
		system("cls");
		if (highScore < score)
		{
			wcout << "*** HIGH SCORE ACHIEVED! ***\n";

			wofstream scoreOutp("./media/score.txt");
			scoreOutp << score;
			scoreOutp.close();
		}
		wcout << "YOUR SCORE: " << to_wstring(score) << "\n";
		wcout << "HIGH SCORE: " << to_wstring(highScore) << "\n";
		wcout << "Thanks for playing!\nTry again? (Y/N)\n\n> ";
		wcin >> userInput;

		transform(userInput.begin(), userInput.end(), userInput.begin(), ::tolower);

		if (userInput == L"y")
		{
			running		= false;
			selected	= true;
		}
		else if (userInput == L"n")
		{
			running		= false;
			gameOpen	= false;

			selected	= true;
		}
		else
		{
			selected = false;
			wcout << "\n[!] Invalid option";
			cin.clear();
			cin.ignore(10000, '\n');
			Sleep(1000);
		}
	}	
}

/// <summary>
/// Gets if a specified file exists.
/// </summary>
/// <param name="fName">File name</param>
/// <returns>True if the file exists</returns>
bool Game::GetIfFileExists(wstring fName)
{
	wifstream file(fName);

	return (file.good());
}