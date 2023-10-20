#include <windows.h>

#include "..\h\env.h"
#include "..\h\game.h"

int Gold::GetValue()
{
	return (value);
}

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

	colour.foreground = 8;

	switch (light)
	{
	case BRIGHT:
		colour.foreground = BLACK; //WAS BLACK
		colour.background = GREY;
		break;
	case MEDIUM:
		colour.foreground = GREY;
		colour.background = BLACK; //WAS GREY
		break;
	case DARK:
		colour.foreground = BLACK; //WAS GREY
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