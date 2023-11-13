#include "..\h\entity.h"
#include "..\h\game.h"
#include <iostream>

using namespace std;

Entity::Entity(int x, int y)
{
	position.x = x;
	position.y = y;

	colour.foreground = DARK_WHITE;
	colour.background = BLACK;
	passable = true;
	symbol = ' ';
}

/// <summary>
/// Updates the keyboard character an Entity of this type should be displayed with.
/// </summary>
/// <param name="symbol">The character to use to represent this Entity</param>
void Entity::SetSymbol(char symbol)
{
	this->symbol = symbol;
}

/// <summary>
/// Updates the colour (foreground and background) an entity should be displayed with.
/// </summary>
/// <param name="fg">Foreground colour</param>
/// <param name="bg">Background colour</param>
void Entity::SetColour(int fg, int bg)
{
	colour.foreground = fg;
	colour.background = bg;
}

/// <summary>
/// Returns the current map position this Entity is located at.
/// </summary>
/// <returns>A Position (x, y coordinate)</returns>
Entity::Position Entity::GetPosition()
{
	return (position);
}

/// <summary>
/// Returns the current symbol being used to display this Entity.
/// </summary>
/// <returns>The character in use to represent this Entity</returns>
char Entity::GetSymbol()
{
	return (symbol);
}

/// <summary>
/// Returns the colour of this Entity.
/// </summary>
/// <returns>Colour object (foreground and background) of the Entity</returns>
Entity::Colour Entity::GetColour()
{
	return (colour);
}

/// <summary>
/// Returns whether this Entity currently has a manually assigned background colour.
/// </summary>
/// <returns>True if a background colour is assigned</returns>
bool Entity::HasBackgroundColour()
{
	bool hasBackground = false;

	if (this->colour.background != 0)
	{
		hasBackground = true;
	}

	return (hasBackground);
}

/// <summary>
/// Returns whether the Entity is "passable" or not - i.e. has collision
/// </summary>
/// <returns>True if collisions are off for this Entity</returns>
bool Entity::GetIfPassable()
{
	return (passable);
}

/// <summary>
/// Outputs an Entity symbol to the map wherever the cursor currently is.
/// </summary>
void Entity::DrawEntity()
{
	wcout << symbol;
}

/// <summary>
/// Calculates the position a Character will be at given a certain Movement type.
/// </summary>
/// <param name="move">The Movement type requested (up/down/left/right)</param>
/// <returns>The resulting position for this Character, given the Movement</returns>
Entity::Position Character::CalculatePos(Movement move)
{
	Position pos = position;

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

/// <summary>
/// Deletes this Entity by freeing it from memory.
/// </summary>
void Entity::Destroy()
{
	delete this;
}

/// <summary>
/// "Moves" a Character by assigning it a new current position.
/// </summary>
/// <param name="pos">Position to update current with</param>
void Character::Move(Position pos)
{
	position = pos;
}