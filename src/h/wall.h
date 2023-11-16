// wall.h - classes for walls on the map, including border walls and in-map walls

#ifndef WALL_H
#define WALL_H

#include <vector>

#include "entity.h"

using namespace std;

/// <summary>
/// Defines walls (map boundaries).
/// </summary>
class Wall : public Entity
{
public:
	Wall(int x, int y) : Entity(x, y)
	{
		passable = false;

		colour.foreground = WHITE;
		colour.background = WHITE;
	}

	virtual wstring SaveDetails();
};

/// <summary>
/// Defines wall blocks, which are in-game walls consisting of multiple single-block Wall objects.
/// </summary>
class WallBlock : public Wall
{
public:
	enum Orientation	{ HORIZONTAL, VERTICAL };
	enum Size			{ LARGE, SMALL };

	WallBlock(int x, int y, Orientation ori, Size size) : Wall(x, y)
	{
		orientation = ori;
		this->size	= size;
		SetLength(size);
		SetBlocks();
	}

	virtual	wstring SaveDetails();

	Position	GetEndPosition();
	Wall*		GetWallBlock(int idx);
	int			GetLength();
	Orientation	GetOrientation();
	bool		GetIfAdjacent(Position pos);
	bool		GetIfAtEdge(Position pos);

	static int	GetWallBlockSize(Size sz, Orientation ori);

	static const int wallGap	= 2;

private:
	Position		endPosition;
	vector<Wall*>	walls;
	Orientation		orientation;
	int				length;
	Size			size;

	WallBlock();
	void SetBlocks();
	void SetLength(Size size);
};

#endif