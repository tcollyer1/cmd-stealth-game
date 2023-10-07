// entity.h

#ifndef ENTITY_H
#define ENTITY_H

#define	ALERT_LEVEL_MAX	3
#define GAME_HEIGHT		20
#define GAME_WIDTH		50

/// <summary>
/// Basic Entity class - represents any interactible object on the player map, including players, enemies, walls, tiles, treasure etc.
/// </summary>
class Entity
{
public:
	typedef struct
	{
		int x;
		int y;
	} Position;

	char			GetSymbol();
	int				GetColour();

	bool			GetIfPassable();

	Position		GetPosition();

	void			DrawEntity();

protected:
	Position		position;
	char			symbol;
	int				colour;
	bool			passable;	// Indicates whether this entity can be passed through by other entities (player/enemy), such as floor

	void			SetSymbol(char symbol);
	void			SetColour(int colour);

	bool			GetIfAtPosition(Position pos);
};

/// <summary>
/// Inherits from Entity. Class that offers basic behaviour of any "human" object on the map - either the player or an enemy.
/// </summary>
class Character : public Entity
{
public:
	enum Movement { UP, DOWN, LEFT, RIGHT, NOTHING };	// Current entity movement direction

	Character(int x, int y);
	void TakeDamage();
	void Attack();
	void Move();
	void Die();

private:
	int			health;
	int			maxHealth;

	bool		isAlive;
};

#endif