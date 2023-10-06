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
	Treasure() : opened(false)
	{
		passable = false;
	}

private:
	bool opened;
};

/// <summary>
/// Level exit.
/// </summary>
class Exit : public Entity
{
public:
	Exit() : triggered(false)
	{
		passable = true;
	}

private:
	bool triggered;
};

class Wall : public Entity
{
public:
	Wall();
};

/// <summary>
/// Class for floor tiles.
/// </summary>
class Tile : public Entity
{
public:
	enum class TerrainType { HARD, SOFT };
	enum class LightLevel { BRIGHT, MEDIUM, DARK };

	Tile(Position position, bool isWalkable, TerrainType terrain, LightLevel lightLevel) : position(position), isWalkable(isWalkable), terrain(terrain), lightLevel(lightLevel)
	{
		passable = true;
	}

	bool GetIfWalkable();

private:
	Tile();
	Position	position;
	bool		isWalkable;
	TerrainType	terrain;
	LightLevel	lightLevel;
};

#endif