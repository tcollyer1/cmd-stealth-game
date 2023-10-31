#include <windows.h>

#include "..\h\wall.h"
#include "..\h\game.h"

void WallBlock::SetBlocks()
{
	Wall* pWall = new Wall(position.x, position.y);
	walls.push_back(pWall);

	if (orientation == HORIZONTAL)
	{
		for (int i = 1; i <= length; i++)
		{
			Wall* pWall = new Wall(position.x + i, position.y);
			walls.push_back(pWall);
		}

		endPosition.x = position.x + length;
		endPosition.y = position.y;
	}
	else
	{
		for (int i = 1; i < length; i++)
		{
			Wall* pWall = new Wall(position.x, position.y + i);
			walls.push_back(pWall);
		}

		endPosition.x = position.x;
		endPosition.y = position.y + length;
	}
}

void WallBlock::SetLength(Size size)
{
	if (orientation == HORIZONTAL)
	{
		length = (size == LARGE ? (GameMap::width / 2) - 2 : (GameMap::width / 4) - 2);
	}
	else
	{
		length = (size == LARGE ? (GameMap::height / 2) - 2 : (GameMap::height / 4) - 2);
	}
}

Entity::Position WallBlock::GetEndPosition()
{
	return (endPosition);
}

Wall* WallBlock::GetWallBlock(int idx)
{
	int actualIdx = idx;

	if (idx > walls.size())
	{
		actualIdx = walls.size() + 1;
	}

	Wall* w = walls[actualIdx];

	return (w);
}

int WallBlock::GetLength()
{
	return (length);
}