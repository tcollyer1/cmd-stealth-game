#include "..\h\player.h"

void Player::IncrementGold(int amount)
{
	gold += amount;
}

int Player::GetGold()
{
	return (gold);
}