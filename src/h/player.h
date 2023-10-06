// player.h

#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

#define MAX_DETECTION 3

class Player : public Entity
{
public:
	Player(int x, int y) : Entity(x, y), maxDetection(MAX_DETECTION)
	{
		currDetection = maxDetection;
	}

private:
	int currDetection;
	int maxDetection;
};

#endif