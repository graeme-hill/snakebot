#include "inyourface.hpp"
#include "../movement.hpp"
#include "../astar.hpp"

#include <functional>
#include <unordered_map>

// Create with no specific target in mind.
InYourFace::InYourFace() : _target(nullptr)
{ }

// Create with a specific target.
InYourFace::InYourFace(Snake *target) : _target(target)
{ }

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

void InYourFace::start(std::string /*id*/)
{
}

MaybeDirection bestCutoff(GameState &state, Snake *target)
{
    if (target == nullptr)
        return MaybeDirection::none();

    Point th = target->head();
    Point mh = state.mySnake()->head();

    // Cells adjacent to target's head
    Point l = { th.x - 1, th.y };
    Point r = { th.x + 1, th.y };
    Point u = { th.x, th.y + 1 };
    Point d = { th.x, th.y - 1 };

    // Only target empty cells adjacent to enemy head
    bool lEmpty = cellIsEmpty(state, l);
    bool rEmpty = cellIsEmpty(state, r);
    bool uEmpty = cellIsEmpty(state, u);
    bool dEmpty = cellIsEmpty(state, d);

    Path res = Path::none();

    // Calc path to each adjacent cell (some of them will be unavailable and
    // give no path).
    auto lPath = lEmpty ? shortestPath(mh, l, state) : Path::none();
    auto rPath = rEmpty ? shortestPath(mh, r, state) : Path::none();
    auto uPath = uEmpty ? shortestPath(mh, u, state) : Path::none();
    auto dPath = dEmpty ? shortestPath(mh, d, state) : Path::none();

    // Pick the best one.
    if (lPath.size > 0 && (res.size == 0 || lPath.size < res.size))
    {
        res = lPath;
    }

    if (rPath.size > 0 && (res.size == 0 || rPath.size < res.size))
    {
        res = rPath;
    }

    if (uPath.size > 0 && (res.size == 0 || uPath.size < res.size))
    {
        res = uPath;
    }

    if (dPath.size > 0 && (res.size == 0 || dPath.size < res.size))
    {
        res = dPath;
    }

    return res.direction;
}

Snake *getSnakeToAnnoy(GameState &state, Snake *fixedTarget)
{
    // If the creator of this class assigned a fixed target then just use that.
    if (fixedTarget != nullptr)
    {
        return fixedTarget;
    }

    auto myLength = state.mySnake()->length();
    Snake *closestSnake = nullptr;
    Snake *closestSmallerSnake = nullptr;

    for (Snake *enemy : state.enemies())
    {
        if (closestSnake == nullptr || closestSnake->length() > enemy->length())
        {
            closestSnake = enemy;
        }

        bool smaller = enemy->length() < myLength;
        if (smaller && (closestSmallerSnake == nullptr || closestSmallerSnake->length() > enemy->length()))
        {
            closestSmallerSnake = enemy;
        }
    }

    return closestSmallerSnake == nullptr ? closestSnake : closestSmallerSnake;
}

Direction InYourFace::move(GameState &state)
{
    if (state.mySnake()->health > 50)
    {
        Snake *target = getSnakeToAnnoy(state, _target);
        auto cutoffDir = bestCutoff(state, target);

        if (cutoffDir.hasValue)
        {
            return cutoffDir.value;
        }
    }

    auto foodDir = closestFood(state);
    if (foodDir.hasValue)
    {
        return foodDir.value;
    }

    auto alt = notImmediatelySuicidal(state);
    return alt.value;
}
