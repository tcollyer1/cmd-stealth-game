// game.h

#ifndef GAME_H
#define GAME_H

#include "entity.h"
#include "player.h"

/// <summary>
/// Class that stores information about every tile on the map, including player & enemy positions.
/// </summary>
class Map : public Entity
{
public:
	enum class ObjectType { PLAYER, ENEMY, TREASURE };

	Map(int enemies, Player* player, Position treasurePos, Position exitPos);
	void Draw();

private:
	Map();
	bool PlayerAtPosition(Position pos);

	int			enemies;	//	Number of enemies on the map
	Position	treasurePos;
	Position	exitPos;
	Player*		pPlayer;

	int			height;
	int			width;

	// Entity entities[height][width] <-- Need an array of all ENTITIES on the map. Probably need malloc/calloc for this
};

class Game : public Entity
{
public:
	Game();
	void Run();		//	Will keep game loop going

private:
	Map* pMap;

	void Setup();			//	Set up map etc., call in Run()
	void ProcessInput();	// Process user commands
};

#endif