// player.h - class for the Player entity.

#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "env.h"

/// <summary>
/// The player character. Inherits from Character class.
/// </summary>
class Player : public Character
{
public:
	Player(int x, int y, ...) : Character(x, y)
	{
		hasKey				= false;
		hasTreasure			= false;
		passable			= false;
		symbol				= 'P';
		colour.foreground	= WHITE;
		gold				= 0;
		currTile			= NULL;
	}

	virtual wstring SaveDetails();

	void			IncrementGold(int amount);
	int				GetGold();

	void			SetKeyObtained(bool obtained);
	bool			GetKeyObtained();

	void			SetTreasureObtained(bool obtained);
	bool			GetTreasureObtained();

	void			UpdateCurrentTile(Tile* t);
	Tile*			GetCurrentTile();

private:
	bool	hasKey;
	bool	hasTreasure;
	int		gold;
	Tile*	currTile;
};

#endif