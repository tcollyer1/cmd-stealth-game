#ifndef ENEMY_H
#define ENEMY_H

#include <random>

#include "entity.h"

using namespace std;

/// <summary>
/// Main Enemy class to define generic enemy behaviour.
/// </summary>
class Enemy : public Character
{
public:
	enum Direction { NORTH, EAST, SOUTH, WEST };	//	Direction enemy is facing

	Enemy(int x, int y, bool hasKey) : Character(x, y)
	{
		alertLevel		= 0;
		this->hasKey	= hasKey;
		passable		= false;
		dir				= Direction(rand() % 4); // Randomise start direction
		symbol			= 'X'; // Default
		colour			= 13;

		UpdateSymbol();
	}

	int		GetAlertLevel();
	bool	GetIfHasKey();
	void	DrawEntity();

private:
	wstring		strSymbol;
	int			alertLevel;
	bool		hasKey;
	Direction	dir;
	void		UpdateSymbol();

	void		UpdateMapSymbol(Movement move);
};

#endif