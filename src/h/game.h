// game.h

#ifndef GAME_H
#define GAME_H

#include <vector>

#include "entity.h"
#include "player.h"
#include "enemy.h"
#include "env.h"
#include "wall.h"

using namespace std;

/// <summary>
/// Class that stores information about every entity on the map, including player & enemy positions.
/// </summary>
class GameMap
{
public:
	GameMap(int enemies);

	void		SetUpMap();
	void		DrawContent();
	bool		GetIfGameOver();
	void		RequestPlayerMove(Character::Movement move, int currTimeMS);
	void		RequestGoldPickup();
	void		RequestEnemyKO();
	bool		RequestEnemyPickpocket();
	bool		RequestTreasureUnlock();
	void		SetUpEnemyMoves(int currTimeMS);
	void		MoveEnemies();
	void		RedrawMap();

	void		UpdateEnemyAwareness(int currTimeMS);

	void		DestroyEverything();

	static const int	height	= 20;
	static const int	width	= 50;

	Entity::Position topLeft;
	Entity::Position topRight;
	Entity::Position btmLeft;
	Entity::Position btmRight;

private:
	GameMap();
	void	SetEssentialEntities();
	void	WriteEntity(Entity* entity, int background = 0);
	bool	GetIfTraversable(Entity::Position pos, bool updatePlayerTile = false);
	int		GetTileBackground(Entity::Position pos);
	void	CalcRandomMove(Entity::Position& newPos, Character::Movement& move);
	void	CalcSpecificMove(Character::Movement& move, Entity::Position& proposedPos, Entity::Position currPos, Entity::Position& targetPos, bool& reroute, bool recurse = false);

	bool	PlayerIsBehindEnemy(int& enemyIdx);

	bool	PlayerIsNextToTreasure();
	void	UpdateTreasureDisplay();

	void	PlaySoundFX(Tile::TerrainType t);

	void	OutputDetectionStr();

	template<typename T> void	AddEntities(int num, vector<T*> &entitiesVector);
	template<typename T> T*		AddEntity(T* pEntity, bool optionalFlag = false);

	void		AddRandomWalls(int cornerX, int cornerY);
	bool		GetIfWallHere(WallBlock wb);
	WallBlock*	GetWallBlock(Entity::Position pos);
	bool		IsPlayerBehindWall(Entity::Position currPos, Enemy::Direction dir);

	int				numEnemies;	// Number of enemies on the map

	Player*			pPlayer;
	Treasure*		pTreasure;
	Exit*			pExit;

	vector<Entity*>		entities;
	vector<Tile*>		tiles;
	vector<Wall*>		walls;
	vector<WallBlock*>	wallBlocks;
	vector<Enemy*>		enemies;
	vector<Gold*>		gold;
};

/// <summary>
/// Main Game class. Responsible for main game loop, setting up/ending the game and generic game properties
/// </summary>
class Game
{
public:
	Game();
	void Run();		// Will keep game loop going

	static const int	hintLineNo		= 1;
	static const int	goldLineNo		= 2;
	static const int	progressLineNo	= 3;
	static const int	statusLineNo	= 4;
	static const int	alertnessLineNo	= 5;
	static const int	timerLineNo		= 6;

	static const int	treasureReward	= 100;

	static void DisplayText(wstring text, int lineNo, int colour, bool noRewrite = false);

	static int	GetColourCode(int foreground, int background);

	static bool GetIfFileExists(wstring fName);

private:
	GameMap* pMap;

	bool StartMenu();														// Initial screen on startup
	void UpdateMap();														// Set up map etc., call in Run()
	void ProcessStartupInput(bool& selected, bool& isNewGame, bool& exit);	// Process start screen user input
	void ProcessGameInput(int currTimeMS);									// Process user commands
	void GameOver();
	void ShowHelp();

	bool	running;
	bool	gameOpen;
};

#endif