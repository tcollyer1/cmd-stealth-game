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
		symbol = '.';
		break;
	case SOFT:
		symbol = ' ';
		break;
	}

	colour.foreground = 8;

	switch (light)
	{
	case BRIGHT:
		//colour = 112;
		//colour = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;
		//colour = 15; // WHITE
		colour.foreground = 0;
		colour.background = 7;
		break;
	case MEDIUM:
		//colour = 128;
		//colour = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
		//colour = 8; // GREY
		colour.foreground = 0;
		colour.background = 8;
		break;
	case DARK:
		//colour = 8;
		//colour = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
		colour.foreground = 8;
		colour.background = 0;
		break;
	}
}