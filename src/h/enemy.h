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

	enum AlertLevel { UNAWARE, SUSPICIOUS, SPOTTED };

	Enemy(int x, int y, bool hasKey = false) : Character(x, y)
	{
		alertLevel			= UNAWARE;
		this->hasKey		= hasKey;
		passable			= false;
		dir					= Direction(rand() % 4); // Randomise start direction
		symbol				= ' '; // Default
		colour.foreground	= 13;
		nextPos.x			= 0;
		nextPos.y			= 0;

		UpdateSymbol();
	}

	AlertLevel		GetAlertLevel();
	bool			GetIfHasKey();
	virtual void	DrawEntity() override;
	Direction		GetDirection();
	virtual void	SetActive(bool active) override;
	void			SetNextPos(Position pos, Movement move);
	Position		GetNextPos();

private:
	wstring		strSymbol;	// For Unicode arrow characters
	AlertLevel	alertLevel;
	bool		hasKey;
	Direction	dir;
	Position	nextPos;

	void		UpdateSymbol();
};

#endif