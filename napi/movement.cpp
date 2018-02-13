#include "movement.hpp"
#include "astar.hpp"

PointDirection closestFood(GameState &state)
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
        return  PointDirection{ true, best.at(0) };
    }
    else
    {
        return PointDirection{ false, Direction::Left };
    }
}