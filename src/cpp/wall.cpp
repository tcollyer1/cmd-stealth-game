#include <windows.h>

#include "..\h\wall.h"
#include "..\h\game.h"

int WallBlock::GetWallBlockSize(Size sz, Orientation ori)
{
	int len = 0;

	switch (sz)
	{
	case SMALL:
		if (ori == HORIZONTAL)
		{
			len = (GameMap::width / 4) - wallGap;
		}
		else
		{
			len = (GameMap::height / 4) - wallGap;
		}
		break;
	case LARGE:
		if (ori == HORIZONTAL)
		{
			len = (GameMap::width / 2) - wallGap;
		}
		else
		{
			len = (GameMap::height / 2) - wallGap;
		}
		break;
	}

	return (len);
}

void WallBlock::SetBlocks()
{
	Wall* pWall = new Wall(position.x, position.y);
	walls.push_back(pWall);

	if (orientation == HORIZONTAL)
	{
		for (int i = 1; i < length; i++)
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
	length = GetWallBlockSize(size, orientation);
}

Entity::Position WallBlock::GetEndPosition()
{
	return (endPosition);
}

WallBlock::Orientation WallBlock::GetOrientation()
{
	return (orientation);
}

/// <summary>
/// Gets if the position of an entity is adjacent to the length of this wall block.
/// </summary>
/// <param name="pos">Position of the entity in question</param>
/// <returns>True if adjacent</returns>
bool WallBlock::GetIfAdjacent(Entity::Position pos)
{
	bool	found	= false;
	bool	isLR	= false;
	int		i		= 0;
	int		leftPos, rightPos;

	switch (orientation)
	{
	case HORIZONTAL:
		if (pos.y == position.y - 1 || pos.y == position.y + 1)
		{
			isLR = true;
		}
		break;
	case VERTICAL:
		if (pos.x == position.x - 1 || pos.x == position.x + 1)
		{
			isLR = true;
		}
		break;
	}

	if (isLR)
	{
		while (i < walls.size() && !found)
		{
			if (orientation == HORIZONTAL)
			{
				if (pos.x == walls[i]->GetPosition().x)
				{
					found = true;
				}
			}
			else
			{
				if (pos.y == walls[i]->GetPosition().y)
				{
					found = true;
				}
			}

			i++;
		}
	}

	return (found);
}

/// <summary>
/// Gets if an entity is at the edge (either end) of this wall block.
/// </summary>
/// <param name="pos">Position of the entity in question</param>
/// <returns>True if at the edge of the wall</returns>
bool WallBlock::GetIfAtEdge(Entity::Position pos)
{
	bool isAtEdge = false;

	switch (orientation)
	{
	case HORIZONTAL:
		if (pos.y == position.y && 
			(pos.x == position.x - 1 || pos.x == endPosition.x + 1))
		{
			isAtEdge = true;
		}
		break;
	case VERTICAL:
		if (pos.x == position.x &&
			(pos.y == position.y - 1 || pos.y == endPosition.y + 1))
		{
			isAtEdge = true;
		}
		break;
	}

	return (isAtEdge);
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