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

	Enemy(int x, int y, bool hasKey = false) : Character(x, y)
	{
		alertLevel		= 0;
		this->hasKey	= hasKey;
		passable		= false;
		dir				= Direction(rand() % 4); // Randomise start direction
		symbol			= ' '; // Default
		colour			= 13;

		UpdateSymbol();
	}

	int				GetAlertLevel();
	bool			GetIfHasKey();
	virtual void	DrawEntity() override;
	void			Move(Position pos, Movement move);

private:
	wstring		strSymbol;	// For Unicode arrow characters
	int			alertLevel;
	bool		hasKey;
	Direction	dir;
	void		UpdateSymbol(); // Not needed?
};

#endif