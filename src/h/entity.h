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

		int x;
		int y;
	};

	struct Colour
	{
		int foreground;
		int background;
	};

	Entity(int x, int y);

	char			GetSymbol();
	Colour			GetColour();

	bool			GetIfPassable();

	bool			HasBackgroundColour();

	Position		GetPosition();

	virtual void	DrawEntity();

	void			Destroy();

protected:
	Position		position;
	char			symbol;
	Colour			colour;
	int				backgroundColour;
	bool			passable;	// Indicates whether this entity can be passed through by other entities (player/enemy), such as floor

	void			SetSymbol(char symbol);
	void			SetColour(int fg, int bg);

private:
	Entity();
};

/// <summary>
/// Inherits from Entity. Class that offers basic behaviour of any "human" object on the map - either the player or an enemy.
/// </summary>
class Character : public Entity
{
public:
	enum Movement { UP, DOWN, LEFT, RIGHT, NOTHING };	// Entity movement direction

	Character(int x, int y) : Entity(x, y)
	{
		isActive = true;
	}
	Position		CalculatePos(Movement move);
	void			Move(Position pos);
	bool			IsActive();
	virtual void	SetActive(bool active) = 0;

protected:
	bool	isActive;

private:
	Character();
};

#endif