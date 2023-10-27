#include <iostream>

#include "..\h\enemy.h"

using namespace std;

void Enemy::DrawEntity()
{	
	wcout << strSymbol;
}

Enemy::AlertLevel Enemy::GetAlertLevel()
{
	return (alertLevel);
}

bool Enemy::GetIfHasKey()
{
	return (hasKey);
}

void Enemy::UpdateSymbol()
{
	switch (dir)
	{
	case NORTH:
		strSymbol = L"↑";
		break;
	case SOUTH:
		strSymbol = L"↓";
		break;
	case WEST:
		strSymbol = L"←";
		break;
	case EAST:
		strSymbol = L"→";
		break;
	default:
		break;
	}

	switch (alertLevel)
	{
	case UNAWARE:
		colour.foreground = GREEN;
		break;
	case SUSPICIOUS:
		colour.foreground = DARK_YELLOW;
		break;
	case SPOTTED:
		colour.foreground = RED;
		break;
	}
}

Enemy::Direction Enemy::GetDirection()
{
	return (dir);
}

void Enemy::SetActive(bool active)
{
	isActive = active;

	if (!active)
	{
		colour.foreground = BLUE;
	}
	else
	{
		colour.foreground = YELLOW;
		//alertLevel = SUSPICIOUS;	// Enemy should be in an alerted state when recovering from takedown
	}
}

void Enemy::SetNextPos(Position pos, Movement move)
{
	nextPos = pos;

	switch (move)
	{
	case UP:
		dir = NORTH;
		break;
	case DOWN:
		dir = SOUTH;
		break;
	case LEFT:
		dir = WEST;
		break;
	case RIGHT:
		dir = EAST;
		break;
	default:
		break;
	}

	UpdateSymbol();
}

Entity::Position Enemy::GetPlayerLastKnownPos()
{
	return (playerLastKnownPos);
}

Enemy::Position Enemy::GetNextPos()
{
	return (nextPos);
}

void Enemy::CheckIfInHearingRange(Entity::Position pos, int timeMS)
{
	int		topY	= position.y - hearingRadius;
	int		btmY	= position.y + hearingRadius;

	int		leftX	= position.x - hearingRadius;
	int		rightX	= position.x + hearingRadius;

	// If player is in hearing range
	if ((pos.x >= leftX)
		&& (pos.x <= rightX)
		&& (pos.y >= topY)
		&& (pos.y <= btmY)
		&& (isActive))
	{
		if (alertLevel == UNAWARE)
		{
			alertLevel		= SUSPICIOUS;
		}

		alertStartTime		= timeMS;
		playerLastKnownPos	= pos;

		if (++confidence == maxConfidence)
		{
			// TODO: Game over...
			alertLevel = SPOTTED;
		}

	}
}

void Enemy::ProcessAlertedState(int timeMS)
{
	if (timeMS >= alertStartTime + alertTimeDuration && alertLevel == SUSPICIOUS)
	{
		confidence		= 0;
		alertLevel		= UNAWARE;
		alertStartTime	= 0;
	}
}