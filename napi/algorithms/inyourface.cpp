#include "inyourface.hpp"
#include "../movement.hpp"
#include "../astar.hpp"

#include <functional>
#include <unordered_map>

Metadata InYourFace::meta()
{
    return {
        "#008888",
        "#FFFFFF",
        "http://garretfitzpatrick.com/storage/in%20your%20face.jpg?__SQUARESPACE_CACHEVERSION=1342854405253",
        "In your face",
        "bam!",
        "pixel",
        "pixel"
    };
}

void InYourFace::start()
{
}

bool cellIsOk(GameState &state, Point p)
{

}

MaybeDirection bestCutoff(GameState &state, Snake *target)
{
    Point th = target->head();
    Point mh state.mySnake()->head();

    Point l = { th.x - 1, th.y };
    Point r = { th.x + 1, th.y };
    Point u = { th.x, th.y + 1 };
    Point d = { th.x, th.y - 1 };

    Path result = Path::none();
    bool found = false;
    uint32_t bestDistance;

    auto lPath = shortestPath(mh, l, state);
    auto rPath = shortestPath(mh, r, state);
    auto uPath = shortestPath(mh, u, state);
    auto dPath = shortestPath(mh, d, state);


}

Snake *getSnakeToAnnoy(GameState &state)
{
    Snake *target = state.enemies().at(0);
    return target;
}

Direction InYourFace::move(GameState &state)
{
    Snake *target = getSnakeToAnnoy(state);
    auto pathToTarget = shortestPath(
        state.mySnake()->head(), target->head(), state);

    if (pathToTarget.direction.hasValue)
    {
        return pathToTarget.direction.value;
    }

    auto foodDir = closestFood(state);
    if (foodDir.hasValue)
    {
        return foodDir.value;
    }

    auto alt = notImmediatelySuicidal(state);
    return alt.value;
}
