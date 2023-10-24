// game.h

#ifndef GAME_H
#define GAME_H

#include <vector>

#include "entity.h"
#include "player.h"
#include "enemy.h"
#include "env.h"

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
	void		RequestPlayerMove(Character::Movement move);
	void		RequestGoldPickup();
	void		RequestEnemyKO();
	bool		RequestEnemyPickpocket();
	void		SetUpEnemyMoves();
	void		MoveEnemies();
	void		RedrawMap();

	static const int	height	= 20;
	static const int	width	= 50;

private:
	GameMap();
	void				WriteEntity(Entity* entity, int background = 0);
	bool				GetIfTraversable(Entity::Position pos, bool updatePlayerTile = false);
	int					GetTileBackground(Entity::Position pos);
	void				CalcRandomMove(Entity::Position& newPos, Character::Movement& move);

	bool	PlayerIsBehindEnemy(int& enemyIdx);

	void	PlaySoundFX(Tile::TerrainType t);

	template<typename T> void AddEntities(int num, vector<T*> &entitiesVector);

	int				numEnemies;	// Number of enemies on the map

	Player*			pPlayer;
	Treasure*		pTreasure;
	Exit*			pExit;

	vector<Entity*> entities;
	vector<Tile*>	tiles;
	vector<Wall*>	walls;
	vector<Enemy*>	enemies;
	vector<Gold*>	gold;
};

/// <summary>
/// Main Game class. Responsible for game loop & setting up/ending game
/// </summary>
class Game
{
public:
	Game();
	void Run();		// Will keep game loop going

	static const int	hintLineNo		= 1;
	static const int	goldLineNo		= 2;
	static const int	statusLineNo	= 4;
	static const int	progressLineNo	= 3;

	static void DisplayText(wstring text, int lineNo, int colour, bool noRewrite = false);

	static int	GetColourCode(int foreground, int background);

private:
	GameMap* pMap;

	bool StartMenu();														// Initial screen on startup
	void UpdateMap();														// Set up map etc., call in Run()
	void ProcessStartupInput(bool& selected, bool& isNewGame, bool& exit);	// Process start screen user input
	void ProcessGameInput();												// Process user commands
	void EndGame();
	void ShowHelp();

	bool	running;
};

#endif