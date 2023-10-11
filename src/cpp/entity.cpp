#include "..\h\entity.h"
#include <iostream>

using namespace std;

void Entity::SetSymbol(char symbol)
{
	this->symbol = symbol;
}

void Entity::SetColour(int colour)
{
	this->colour = colour;
}

Entity::Position Entity::GetPosition()
{
	return (position);
}

char Entity::GetSymbol()
{
	return (symbol);
}

int Entity::GetColour()
{
	return (colour);
}

bool Entity::GetIfPassable()
{
	return (passable);
}

void Entity::DrawEntity()
{
	wcout << symbol;
}

Character::Character(int x, int y)
{
	position.x	= x;
	position.y	= y;
	isAlive		= true;
}

Entity::Position Character::CalculatePos(Movement move)
{
	Position pos = GetPosition();

	switch (move)
	{
	case UP:
		pos.y--;
		break;
	case DOWN:
		pos.y++;
		break;
	case LEFT:
		pos.x--;
		break;
	case RIGHT:
		pos.x++;
		break;
	default:
		break;
	}

	return (pos);
}

// Possibly virtual. Enemies might move differently to the player(?)
void Character::Move(Position pos)
{
	position = pos;
}

void Character::Die()
{
	delete this;
}