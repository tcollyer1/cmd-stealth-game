#include <iostream>

#include "..\h\enemy.h"

using namespace std;

void Enemy::DrawEntity()
{	
	wcout << strSymbol;
}

int Enemy::GetAlertLevel()
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

	// Should replace this with an enum
	switch (alertLevel)
	{
	case 0:
		colour.foreground = 10;
		break;
	case 1:
		colour.foreground = 14;
		break;
	case 2:
		colour.foreground = 12;
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
		colour.foreground = 9;
	}
	else
	{
		colour.foreground = 13;
	}
}

void Enemy::Move(Position pos, Movement move)
{
	position = pos;

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