// entity.h

#ifndef ENTITY_H
#define ENTITY_H

#define MAX_HEALTH	10

class Entity
{
public:
	Entity(int x, int y) : maxHealth(MAX_HEALTH), isAlive(true) {}
	void TakeDamage();
	void Attack();
	void Move();

private:
	int		health;
	int		maxHealth;

	int		position[2];

	bool	isAlive;
};

#endif