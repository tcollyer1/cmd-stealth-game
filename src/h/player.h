// player.h

#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

/// <summary>
/// The player character. Inherits from Character class.
/// </summary>
class Player : public Character
{
public:
	Player(int x, int y) : Character(x, y)
	{
		currDetection	= 0;
		hasKey			= false;
		passable		= false;
		symbol			= 'P';
		colour			= 11;
		gold			= 0;
	}

private:
	int		currDetection;
	bool	hasKey;
	int		gold;
};

#endif