#ifndef ENEMY_H
#define ENEMY_H

#include "entity.h"

/// <summary>
/// Main Enemy class to define generic enemy behaviour.
/// </summary>
class Enemy : public Character
{
public:
	enum Direction { NORTH, EAST, SOUTH, WEST };	//	Direction enemy is facing

	Enemy(int x, int y, bool hasKey) : Character(x, y), alertLevel(0), hasKey(hasKey) {}

private:
	int		alertLevel;
	bool	hasKey;
};

#endif