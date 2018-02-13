#pragma once

#include "snakelib.hpp"

struct PointDirection
{
	bool possible;
	Direction direction;
};

PointDirection closestFood(GameState &state);