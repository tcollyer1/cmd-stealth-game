// entity.h - base Entity class all map entities inherit from, and base Character class (inherits from Entity) that enemies/the player inherits from.

#ifndef ENTITY_H
#define ENTITY_H

#define	ALERT_LEVEL_MAX	2
#define MAX_ENEMIES		5

/// <summary>
/// Basic Entity class - represents any object on the player map, including the player, enemies, walls, tiles, treasure etc.
/// </summary>
class Entity
{
public:
	// Override for ease of comparing/assigning positions
	struct Position
	{
		bool operator==(const Position& otherPos)
		{
			return (x == otherPos.x && y == otherPos.y);
		}

		int x;
		int y;
	};

	// Console colours
	enum Colours
	{
		BLACK = 0,
		DARK_BLUE,
		DARK_GREEN,
		DARK_CYAN,
		DARK_RED,
		DARK_PINK,
		DARK_YELLOW,
		DARK_WHITE,
		GREY,
		BLUE,
		GREEN,
		CYAN,
		RED,
		PINK,
		YELLOW,
		WHITE
	};

	// Console foreground/background colour
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

	Character(int x, int y) : Entity(x, y) {};
	Position		CalculatePos(Movement move);
	void			Move(Position pos);

private:
	Character();
};

#endif