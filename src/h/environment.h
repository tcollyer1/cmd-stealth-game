#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "entity.h"
#include "player.h"

/// <summary>
/// Treasure object that the player must obtain before leaving the map.
/// </summary>
class Treasure : public Entity
{
public:
	Treasure() : opened(false) {}

private:
	bool opened;
};

class Exit : public Entity
{

};

/// <summary>
/// Class for the visual tiles to be displayed on the map, ranging from the floor, walls, treasure icon, and enemy/player icons.
/// </summary>
class Tile : public Entity
{
public:
	enum class TerrainType	{ HARD, SOFT, WALL, CHARACTER };
	enum LightLevel		{ BRIGHT, MEDIUM, DARK, NONE };

	Tile(Position position, bool isWalkable, TerrainType terrain, LightLevel lightLevel) : position(position), isWalkable(isWalkable), terrain(terrain), lightLevel(lightLevel) {}

	bool GetIfWalkable();

private:
	Tile();
	Position	position;
	bool		isWalkable;
	TerrainType	terrain;
	LightLevel	lightLevel;
};

/// <summary>
/// Class that stores information about every tile on the map, including player & enemy positions.
/// </summary>
class Map : public Entity
{
public:
	Map(int enemies, Player& player);	//	Draw the map to the screen

private:
	int			enemies;	//	Number of enemies on the map
	Position	treasurePos;
	Position	exitPos;
	Player*		pPlayer;
	// <-- Need an array of all enemies on the map
};

class Game
{
public:
	Game(int enemies);
	void Run();		//	Will keep game loop going

private:
	// <-- Have a pointer to the map here?
	Game();
	void Setup();	//	Set up map etc., call in Run()
};

#endif