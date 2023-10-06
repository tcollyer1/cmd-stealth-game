#include "..\h\environment.h"

//	Draw map
Map::Map(int enemies, Player& player)
{
	this->enemies	= enemies;
	(*pPlayer)		= player;
}

bool Tile::GetIfWalkable()
{
	return (isWalkable);
}

void Game::Setup()
{

}

void Game::Run()
{

}