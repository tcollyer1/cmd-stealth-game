// env.h - classes for static environment object, such as walls, floor, treasure etc.

#ifndef ENV_H
#define ENV_H

#include "entity.h"

/// <summary>
/// Treasure object that the player must obtain before leaving the map.
/// </summary>
class Treasure : public Entity
{
public:
	Treasure(int x, int y);

private:
	bool opened;
};

/// <summary>
/// Level exit.
/// </summary>
class Exit : public Entity
{
public:
	Exit(int x, int y);

private:
	bool triggered;
};

/// <summary>
/// Defines walls (map boundaries).
/// </summary>
class Wall : public Entity
{
public:
	Wall(int x, int y);
};

/// <summary>
/// Class for floor tiles.
/// </summary>
class Tile : public Entity
{
public:
	enum TerrainType { HARD, SOFT };
	enum LightLevel { BRIGHT, MEDIUM, DARK };

	Tile(Position position, TerrainType terrain, LightLevel lightLevel);

private:
	TerrainType	terrain;
	LightLevel	lightLevel;
};

#endif