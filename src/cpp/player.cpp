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

void Player::SetActive(bool active)
{
	isActive = active;
}