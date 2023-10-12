// entity.h

#ifndef ENTITY_H
#define ENTITY_H

#define	ALERT_LEVEL_MAX	2
#define MAX_ENEMIES		5

/// <summary>
/// Basic Entity class - represents any interactible object on the player map, including players, enemies, walls, tiles, treasure etc.
/// </summary>
class Entity
{
public:
	struct Position
	{
		bool operator==(const Position& otherPos)
		{
			return (x == otherPos.x && y == otherPos.y);
		}

		/*Position& operator=(const Position& otherPos)
		{
			x = otherPos.x;
			y = otherPos.y;

			return *this;
		}*/

		int x;
		int y;
	};

	char			GetSymbol();
	int				GetColour();

	bool			GetIfPassable();

	Position		GetPosition();

	virtual void	DrawEntity();

protected:
	Position		position;
	char			symbol;
	int				colour;
	bool			passable;	// Indicates whether this entity can be passed through by other entities (player/enemy), such as floor

	void			SetSymbol(char symbol);
	void			SetColour(int colour);
};

/// <summary>
/// Inherits from Entity. Class that offers basic behaviour of any "human" object on the map - either the player or an enemy.
/// </summary>
class Character : public Entity
{
public:
	enum Movement { UP, DOWN, LEFT, RIGHT, NOTHING };	// Entity movement direction

	Character(int x, int y);
	Position		CalculatePos(Movement move);
	void			Move(Position pos);
	virtual void	Disable();

private:
	Character();
	bool		isAlive;
};

#endif