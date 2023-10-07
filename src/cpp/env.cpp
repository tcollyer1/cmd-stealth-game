#include "..\h\env.h"

bool Tile::GetIfWalkable()
{
	return (isWalkable);
}

Wall::Wall(int x, int y)
{
	passable	= true;
	position.x	= x;
	position.y	= y;

	SetSymbol('#');
	SetColour(15);
}

Treasure::Treasure(int x, int y)
{
	opened		= false;
	passable	= false;
	position.x	= x;
	position.y	= y;

	SetSymbol('T');
	SetColour(6);
}

Exit::Exit(int x, int y)
{
	triggered	= false;
	passable	= true;
	position.x	= x;
	position.y	= y;

	SetSymbol('E');
	SetColour(13);
}