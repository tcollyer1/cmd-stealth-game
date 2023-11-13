// enemy.h - class for Enemy entities.

#ifndef ENEMY_H
#define ENEMY_H

#include <random>

#include "entity.h"
#include "env.h"

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
		alertLevel				= UNAWARE;
		this->hasKey			= hasKey;
		passable				= false;
		dir						= Direction(rand() % 4); // Randomise start direction
		nextPos.x				= 0;
		nextPos.y				= 0;

		playerLastKnownPos.x	= 0;
		playerLastKnownPos.y	= 0;
		intermediatePos.x		= 0;
		intermediatePos.y		= 0;
		detection				= 0;
		alertStartTime			= 0;
		koStartTime			= 0;

		UpdateSymbol();
	}

	bool			GetIfHasKey();
	virtual void	DrawEntity() override;
	Direction		GetDirection();
	void			SetActive(bool active, int timeMS = 0);
	void			SetNextPos(Position pos, Movement move);
	Position		GetNextPos();
	void			ProcessAlertedState(int timeMS, Position playerPosActual);
	void			ProcessKOState(int timeMS);

	// Player-related functions
	AlertLevel		GetAlertLevel();
	Position		GetPlayerLastKnownPos();
	void			ResetIntermediatePos();
	void			SetIntermediatePos(Position pos);
	void			GetIntermediatePos(Position& pos);
	void			CheckIfInHearingRange(Position pos, int timeMS);
	void			CheckIfInLOS(Position pos, int timeMS, Tile::LightLevel light);
	int				GetDetectionLevel();
	void			ClearDetectionLevel();
	bool			IsActive();

private:
	bool	isActive;
	wstring		strSymbol;	// For Unicode arrow characters
	AlertLevel	alertLevel;
	bool		hasKey;
	Direction	dir;
	Position	nextPos;
	Position	playerLastKnownPos;
	Position	intermediatePos;
	const int	hearingRadius		= 4; // Was 3
	const int	lineOfSight			= 6; // Was 6
	int			detection;
	int			alertStartTime;
	int			koStartTime;

	const int	alertTimeDuration	= 5000;
	const int	koTimeDuration		= 10000;
	const int	maxDetection		= 6;

	void		UpdateSymbol();
};

#endif