#include <windows.h>
#include <string>

#include "..\h\env.h"
#include "..\h\game.h"

wstring Exit::SaveDetails()
{
	wstring str = to_wstring(position.x) + L"," + to_wstring(position.y) + L"\n";

	return (str);
}

wstring Treasure::SaveDetails()
{
	wstring str = to_wstring(position.x) + L"," + to_wstring(position.y) + L"," + to_wstring(found) + L"\n";

	return (str);
}

/// <summary>
/// Marks the Treasure on the map once the Player has successfully found it; otherwise keeps it hidden.
/// </summary>
/// <param name="found">True if the Player has found the Treasure's location</param>
void Treasure::MarkAsFound(bool found)
{
	if (found)
	{
		symbol = 'X';
		colour.foreground = DARK_RED;
	}
	else
	{
		symbol = ' ';
		colour.foreground = BLACK;
		colour.background = BLACK;
	}

	this->found = found;
}

/// <summary>
/// Returns the monetary value this Gold piece has.
/// </summary>
/// <returns>The value of this Gold object</returns>
int Gold::GetValue()
{
	return (value);
}

wstring Gold::SaveDetails()
{
	wstring str = to_wstring(position.x) + L"," + to_wstring(position.y) + L"\n";

	return (str);
}

wstring Tile::SaveDetails()
{
	wstring str = to_wstring(position.x) + L"," + to_wstring(position.y) + L"," + to_wstring(terrain) + L"," + to_wstring(lightLevel) + L"\n";

	return (str);
}

/// <summary>
/// Generates the correct symbol for a particular Tile based on its properties.
/// </summary>
/// <param name="terrain">The TerrainType (hard/soft) of the tile</param>
/// <param name="light">The light level (dark/medium/light) of the tile</param>
void Tile::DetermineSymbol(TerrainType terrain, LightLevel light)
{
	switch (terrain)
	{
	case HARD:
		symbol = '*';
		break;
	case SOFT:
		symbol = '.';
		break;
	}

	switch (light)
	{
	case BRIGHT:
		colour.foreground = BLACK;
		colour.background = GREY;
		break;
	case MEDIUM:
		colour.foreground = GREY;
		colour.background = BLACK;
		break;
	case DARK:
		colour.foreground = BLACK;
		colour.background = BLACK;
		break;
	}
}

Tile::LightLevel Tile::GetLightLevel()
{
	return (lightLevel);
}

Tile::TerrainType Tile::GetTerrainType()
{
	return (terrain);
}