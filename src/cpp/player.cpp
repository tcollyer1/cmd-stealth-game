#include "..\h\player.h"

void Player::IncrementGold(int amount)
{
	gold += amount;
}

int Player::GetGold()
{
	return (gold);
}

void Player::SetKeyObtained(bool obtained)
{
	hasKey = obtained;
}

bool Player::GetKeyObtained()
{
	return (hasKey);
}

void Player::SetTreasureObtained(bool obtained)
{
	hasTreasure = obtained;
}

bool Player::GetTreasureObtained()
{
	return (hasTreasure);
}

void Player::SetActive(bool active)
{
	isActive = active;
}

void Player::UpdateCurrentTile(Tile* t)
{
	currTile = t;
}

Tile* Player::GetCurrentTile()
{
	return (currTile);
}