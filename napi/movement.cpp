#include "movement.hpp"
#include "astar.hpp"
#include "snakelib.hpp"

MaybeDirection closestFood(GameState &state)
{
    Snake *me = state.mySnake();
    std::vector<Direction> best;
    bool foundAnything = false;

    for (auto food : state.food())
    {
        std::vector<Direction> myPath = shortestPath(me->head(), food, state);

        if (myPath.empty())
        {
            continue;
        }

        if (myPath.size() >= best.size() && foundAnything)
        {
            continue;
        }

        foundAnything = true;
        best = myPath;
    }

    if (!best.empty())
    {
        return  MaybeDirection{ true, best.at(0) };
    }
    else
    {
        return MaybeDirection{ false, Direction::Left };
    }
}

bool is180(Point p, GameState &state)
{
    // If length of snake is 1 then nothing to worry about (... I think... might wanna test that)
    if (state.mySnake()->length() <= 1)
    {
        return false;
    }

    Point neck = state.mySnake()->parts.at(1);

    return neck == p;
}

bool isCellOk(Point p, GameState &state)
{
    return !outOfBounds(p, state)
        && state.map().turnsUntilVacant(p) == 0
        && !is180(p, state);
}

std::vector<Direction> notImmediatelySuicidalMoves(GameState &state)
{
    Point myHead = state.mySnake()->head();
    uint32_t x = myHead.x;
    uint32_t y = myHead.y;
    std::vector<Direction> moves;

    if (isCellOk({x - 1, y}, state))
    {
        moves.push_back(Direction::Left);
    }
    if (isCellOk({x + 1, y}, state))
    {
        moves.push_back(Direction::Right);
    }
    if (isCellOk({x, y - 1}, state))
    {
        moves.push_back(Direction::Up);
    }
    if (isCellOk({x, y + 1}, state))
    {
        moves.push_back(Direction::Down);
    }

    return moves;
}

MaybeDirection notImmediatelySuicidal(GameState &state)
{
    std::vector<Direction> moves = notImmediatelySuicidalMoves(state);
    if (moves.empty())
    {
        return MaybeDirection{ false, Direction::Left };
    }
    else
    {
        return MaybeDirection{ true, moves.at(0) };
    }
}