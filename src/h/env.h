// env.h - classes for environment objects, such as floor, treasure, exit

#ifndef ENV_H
#define ENV_H

#include "entity.h"
#include <random>

class Gold : public Entity
{
public:
	Gold(int x, int y) : Entity(x, y)
	{
		passable			= true;

		symbol				= 'o';
		colour.foreground	= YELLOW;
		value				= rand() % 10 + 1;	// Random value between 1 and 10
	}

	int		GetValue();

private:
	int value;
};

/// <summary>
/// Treasure object that the player must obtain before leaving the map.
/// </summary>
class Treasure : public Entity
{
public:
	Treasure(int x, int y, ...) : Entity(x, y)
	{
		opened		= false;
		passable	= false;		

		MarkAsFound(false);
	}

	void MarkAsFound(bool found);

private:
	bool opened;
	bool found;
};

/// <summary>
/// Level exit.
/// </summary>
class Exit : public Entity
{
public:
	Exit(int x, int y, ...) : Entity(x, y)
	{
		triggered			= false;
		passable			= true;

		symbol				= 'E';
		colour.foreground	= PINK;
	}

private:
	bool triggered;
};

/// <summary>
/// Class for floor tiles.
/// </summary>
class Tile : public Entity
{
public:
	enum TerrainType	{ HARD, SOFT };
	enum LightLevel		{ BRIGHT, MEDIUM, DARK };

	Tile(int x, int y, TerrainType terrain, LightLevel lightLevel) : Entity(x, y)
	{
		this->terrain = terrain;
		this->lightLevel = lightLevel;
		passable = true;

		DetermineSymbol(terrain, lightLevel);		
	}	

	TerrainType GetTerrainType();
	LightLevel	GetLightLevel();

private:
	TerrainType	terrain;
	LightLevel	lightLevel;

	void DetermineSymbol(TerrainType terrain, LightLevel light);
};

#endif