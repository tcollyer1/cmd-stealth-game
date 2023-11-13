#include <iostream>

#include "..\h\enemy.h"
#include "..\h\game.h"

using namespace std;

/// <summary>
/// Overriden DrawEntity() function - draws unicode arrow character using a wstring object instead of a char.
/// </summary>
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

/// <summary>
/// Updates enemy appearance based on their current direction and alert level.
/// </summary>
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

/// <summary>
/// Handles reactivating an enemy after the player has knocked them out.
/// </summary>
/// <param name="timeMS">Current time elapsed in ms</param>
void Enemy::ProcessKOState(int timeMS)
{
	if (timeMS >= koStartTime + koTimeDuration && !isActive)
	{
		isActive = true;
		alertLevel = SUSPICIOUS;
		if (++detection == maxDetection)
		{
			// Game over...
			alertLevel = SPOTTED;
		}

		alertStartTime = timeMS;
	}
}

/// <summary>
/// Sets an enemy to active, or inactive based on whether they've woken up or been knocked out.
/// </summary>
/// <param name="active">True to set the enemy to active; false to render them inactive</param>
/// <param name="timeMS">Current time elapsed in ms</param>
void Enemy::SetActive(bool active, int timeMS)
{
	isActive = active;

	if (!active)
	{
		colour.foreground = BLUE;
		passable = true;
		koStartTime = timeMS;
	}
	else
	{
		koStartTime = 0;
		passable = false;
		colour.foreground = GREEN;
		alertLevel = UNAWARE;
	}
}

/// <summary>
/// Assigns the position the enemy will move to on the next enemy movement iteration, updating their intended direction.
/// </summary>
/// <param name="pos">Position to move to next</param>
/// <param name="move">Movement direction</param>
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

/// <summary>
/// Returns the player's last known position according to this Enemy.
/// </summary>
/// <returns>A Position (x, y) relating to the player's last known position.</returns>
Entity::Position Enemy::GetPlayerLastKnownPos()
{
	return (playerLastKnownPos);
}

/// <summary>
/// Gets the enemy's intermediate position - this being a rerouted position to allow an enemy to navigate a basic wall more reliably.
/// </summary>
/// <param name="pos">Stores the the enemy's intermediate position, if any</param>
void Enemy::GetIntermediatePos(Position& pos)
{
	if ((!intermediatePos.x == 0) && (!intermediatePos.y == 0))
	{
		pos = intermediatePos;
	}
}

/// <summary>
/// Clears the intermediate position for this enemy.
/// </summary>
void Enemy::ResetIntermediatePos()
{
	intermediatePos.x = 0;
	intermediatePos.y = 0;
}

/// <summary>
/// Assigns an intermediate position to an enemy, that they will attempt to reach before reaching a specific (player last known) position. 
/// Used to help with navigating around a WallBlock object.
/// </summary>
/// <param name="pos">The position to assign as the intermediate position</param>
void Enemy::SetIntermediatePos(Position pos)
{
	intermediatePos = pos;
}

/// <summary>
/// Returns the next position the enemy will move to.
/// </summary>
/// <returns>A Position (x, y) relating to the enemy's next move.</returns>
Enemy::Position Enemy::GetNextPos()
{
	return (nextPos);
}

/// <summary>
/// Gets and does relevant alert level management if the player, given their current position and assuming they have stepped on a 
/// loud tile, is within the enemy's hearing range.
/// </summary>
/// <param name="pos">Player position</param>
/// <param name="timeMS">Current time elapsed - to update how long an enemy should continue to be alerted for</param>
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
			alertLevel	= SUSPICIOUS;
		}

		alertStartTime		= timeMS;
		playerLastKnownPos	= pos;

		if (++detection == maxDetection)
		{
			alertLevel = SPOTTED;
		}
	}
}

/// <summary>
/// Checks if an entity (namely the player) is within an enemy's line of sight, and handles the enemy's reaction (alert level/detection) accordingly.
/// </summary>
/// <param name="pos">Position of the Entity to check</param>
/// <param name="timeMS">Current time elapsed in ms</param>
/// <param name="light">The light level of the tile the Entity to check is currently positioned on</param>
void Enemy::CheckIfInLOS(Entity::Position pos, int timeMS, Tile::LightLevel light)
{
	bool inLOS		= false;
	bool spotted	= false;

	// For dark tiles: completely invisible
	// For medium tiles: go into alert at all distances within line of sight range
	// For light tiles: go into alert at 2nd half of distance within line of sight range, immediate game over if in first half

	if (light != Tile::DARK)
	{
		switch (dir)
		{
		case NORTH:
			if (((pos.x == position.x)
				|| (pos.x == position.x - 1)
				|| (pos.x == position.x + 1))
				&& (pos.y >= position.y - lineOfSight)
				&& (pos.y <= position.y)
				&& (isActive))
			{
				inLOS = true;

				if ((pos.y >= position.y - (lineOfSight / 2)) && (light == Tile::BRIGHT))
				{
					spotted = true;
				}
			}
			break;
		case SOUTH:
			if (((pos.x == position.x)
				|| (pos.x == position.x - 1)
				|| (pos.x == position.x + 1))
				&& (pos.y <= position.y + lineOfSight)
				&& (pos.y >= position.y)
				&& (isActive))
			{
				inLOS = true;

				if ((pos.y <= position.y + (lineOfSight / 2)) && (light == Tile::BRIGHT))
				{
					spotted = true;
				}
			}
			break;
		case WEST:
			if (((pos.y == position.y)
				|| (pos.y == position.y - 1)
				|| (pos.y == position.y + 1))
				&& (pos.x >= position.x - lineOfSight)
				&& (pos.x <= position.x)
				&& (isActive))
			{
				inLOS = true;

				if ((pos.x >= position.x - (lineOfSight / 2)) && (light == Tile::BRIGHT))
				{
					spotted = true;
				}
			}
			break;
		case EAST:
			if (((pos.y == position.y)
				|| (pos.y == position.y - 1)
				|| (pos.y == position.y + 1))
				&& (pos.x <= position.x + lineOfSight)
				&& (pos.x >= position.x)
				&& (isActive))
			{
				inLOS = true;

				if ((pos.x <= position.x + (lineOfSight / 2)) && (light == Tile::BRIGHT))
				{
					spotted = true;
				}
			}
			break;
		}
	}	

	if (spotted)
	{
		// Game over...
		detection	= maxDetection;
		alertLevel	= SPOTTED;
	}
	else if (inLOS)
	{
		if (alertLevel == UNAWARE)
		{
			alertLevel = SUSPICIOUS;
		}

		alertStartTime = timeMS;
		playerLastKnownPos = pos;

		if (++detection == maxDetection)
		{
			// Game over...
			alertLevel = SPOTTED;
		}
	}
}

/// <summary>
/// Handles the alerted state of an enemy, when their suspicion expires and whether they've directly spotted the player.
/// </summary>
/// <param name="timeMS">Current time elapsed in ms</param>
/// <param name="playerPosActual">The player's current position</param>
void Enemy::ProcessAlertedState(int timeMS, Position playerPosActual)
{
	bool playerFound = false;

	// If standing in front of the player
	switch (dir)
	{
	case NORTH:
		if (playerPosActual.x == position.x && playerPosActual.y == position.y - 1)
		{
			playerFound = true;
		}
		break;
	case SOUTH:
		if (playerPosActual.x == position.x && playerPosActual.y == position.y + 1)
		{
			playerFound = true;
		}
		break;
	case EAST:
		if (playerPosActual.x == position.x + 1 && playerPosActual.y == position.y)
		{
			playerFound = true;
		}
		break;
	case WEST:
		if (playerPosActual.x == position.x - 1 && playerPosActual.y == position.y)
		{
			playerFound = true;
		}
		break;
	}

	if (playerFound)
	{
		detection	= maxDetection;
		alertLevel	= SPOTTED;
	}

	if (timeMS >= alertStartTime + alertTimeDuration && alertLevel == SUSPICIOUS)
	{
		ClearDetectionLevel();
		ResetIntermediatePos();
		alertLevel		= UNAWARE;
		alertStartTime	= 0;

	}
}

int Enemy::GetDetectionLevel()
{
	return (detection);
}

void Enemy::ClearDetectionLevel()
{
	detection = 0;
}

/// <summary>
/// Returns whether the current Enemy is active (not knocked out).
/// </summary>
/// <returns>True if active</returns>
bool Enemy::IsActive()
{
	return (isActive);
}