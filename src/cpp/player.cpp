#include "..\h\player.h"

wstring Player::SaveDetails()
{
	wstring str = to_wstring(position.x) + L"," + to_wstring(position.y) + L"," + to_wstring(hasKey) + L"," + to_wstring(hasTreasure) + L","
		+ to_wstring(gold) + L"\n";

	return (str);
}

/// <summary>
/// Increments player gold count by the specified amount.
/// </summary>
/// <param name="amount">Amount of gold to increment by</param>
void Player::IncrementGold(int amount)
{
	gold += amount;
}

int Player::GetGold()
{
	return (gold);
}

/// <summary>
/// Toggles whether the key to the Treasure has been obtained or not.
/// </summary>
/// <param name="obtained">True if obtained; false if not</param>
void Player::SetKeyObtained(bool obtained)
{
	hasKey = obtained;
}

/// <summary>
/// Gets if the Player has the key or not.
/// </summary>
/// <returns>True if key has been collected by the Player</returns>
bool Player::GetKeyObtained()
{
	return (hasKey);
}

/// <summary>
/// Toggles whether the Treasure has been obtained or not.
/// </summary>
/// <param name="obtained">True if obtained; false if not</param>
void Player::SetTreasureObtained(bool obtained)
{
	hasTreasure = obtained;
}

/// <summary>
/// Gets if the Player has the Treasure or not.
/// </summary>
/// <returns>True if Treasure has been collected by the Player</returns>
bool Player::GetTreasureObtained()
{
	return (hasTreasure);
}

/// <summary>
/// Updates the pointer to the current Tile the Player is standing on.
/// </summary>
/// <param name="t">Pointer to the Tile object at the Player's position</param>
void Player::UpdateCurrentTile(Tile* t)
{
	currTile = t;
}

/// <summary>
/// Retrieves information about the Tile the Player is currently standing on.
/// </summary>
/// <returns>Pointer to the Tile object the Player is on</returns>
Tile* Player::GetCurrentTile()
{
	return (currTile);
}