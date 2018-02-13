#pragma once

#include "snakelib.hpp"

struct MaybeDirection
{
	bool hasValue;
	Direction value;
};

MaybeDirection closestFood(GameState &state);

// MaybeDirection bestFood(GameState &state);

std::vector<Direction> notImmediatelySuicidalMoves(GameState &state);

MaybeDirection notImmediatelySuicidal(GameState &gameState);

MaybeDirection chaseTail(GameState &state);