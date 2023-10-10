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
class Map
{
public:
	Map(int enemies, Player* player, Treasure* treasure, Exit* exit);
	void		SetUpMap();
	void		DrawContent();
	void		RequestMove(Character::Movement move);

private:
	Map();
	void	WriteEntity(Entity* entity);
	bool	GetIfTraversable(Entity::Position pos);

	int				numEnemies;	//	Number of enemies on the map

	Player*			pPlayer;
	Treasure*		pTreasure;
	Exit*			pExit;

	vector<Entity*> entities;
	vector<Tile>	tiles;
	vector<Wall>	walls;
	vector<Enemy>	enemies;

	int				height;
	int				width;
};

/// <summary>
/// Main Game class. Responsible for game loop & setting up/ending game
/// </summary>
class Game
{
public:
	Game();
	void Run();		// Will keep game loop going

private:
	Map* pMap;

	void Setup();			// Set up map etc., call in Run()
	void ProcessInput();	// Process user commands
	void EndGame();

	bool running;
};

#endif