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
		colour = 10;
		break;
	case 1:
		colour = 14;
		break;
	case 2:
		colour = 12;
		break;
	}
}

// Temp - need to rework this to be useful for the Map to set the position when moving
void Enemy::UpdateMapSymbol(Movement move)
{
	switch (move)
	{
	case UP:
		symbol = '↑';
		break;
	case DOWN:
		symbol = '↓';
		break;
	case LEFT:
		symbol = '←';
		break;
	case RIGHT:
		symbol = '→';
		break;
	default:
		break;
	}

	// Should replace this with an enum
	switch (alertLevel)
	{
	case 0:
		colour = 6;
		break;
	case 1:
		colour = 14;
		break;
	case 2:
		colour = 12;
		break;
	}
}