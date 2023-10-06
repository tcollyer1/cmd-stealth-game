#include "..\h\entity.h"

void Entity::SetSymbol(char symbol)
{
	this->symbol = symbol;
}

Entity::Position Entity::GetPosition()
{
	return (position);
}

char Entity::GetSymbol()
{
	return (symbol);
}

bool Entity::GetIfPassable()
{
	return (passable);
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