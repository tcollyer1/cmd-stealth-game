#include "..\h\env.h"

bool Tile::GetIfWalkable()
{
	return (isWalkable);
}

Tile::Tile(Position position, bool isWalkable, TerrainType terrain, LightLevel lightLevel)
{
	this->position = position;
	this->isWalkable = isWalkable;
	this->terrain = terrain;
	this->lightLevel = lightLevel;
	passable = true;
	symbol = ' ';
	colour = 17; // Eventually to be determined by tile type
}

Wall::Wall(int x, int y)
{
	passable	= false;
	position.x	= x;
	position.y	= y;

	symbol = '#';
	colour = 15;
}

Treasure::Treasure(int x, int y)
{
	opened		= false;
	passable	= false;
	position.x	= x;
	position.y	= y;

	symbol = 'T';
	colour = 6;
}

Exit::Exit(int x, int y)
{
	triggered	= false;
	passable	= true;
	position.x	= x;
	position.y	= y;

	symbol = 'E';
	colour = 13;
}