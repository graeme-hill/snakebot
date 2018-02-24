#pragma once

#include "snakelib.hpp"

MaybeDirection closestFood(GameState &state);

MaybeDirection bestFoodBruteForce(GameState &state);
MaybeDirection bestFood(GameState &state);
MaybeDirection bestFoodSorted(GameState &state);

DirectionSet notImmediatelySuicidalMoves(GameState &state);

DirectionSet safeMoves(GameState &state);

DirectionSet riskyMoves(GameState &state);

MaybeDirection notImmediatelySuicidal(GameState &gameState);

MaybeDirection chaseTail(GameState &state);
