#include "..\h\entity.h"

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

void Character::Move()
{

}

void Character::Die()
{
	delete this;
}