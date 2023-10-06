#include "..\h\env.h"

bool Tile::GetIfWalkable()
{
	return (isWalkable);
}

Wall::Wall()
{
	passable = true;
}