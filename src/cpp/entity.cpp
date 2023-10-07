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
	cout << symbol;
}

Character::Character(int x, int y)
{
	position.x	= x;
	position.y	= y;
	maxHealth	= 10;
	isAlive		= true;
	health		= maxHealth;
}

void Character::TakeDamage()
{

}

void Character::Attack()
{

}

// Possibly virtual. Enemies will move differently to the player
void Character::Move()
{

}

void Character::Die()
{
	delete this;
}