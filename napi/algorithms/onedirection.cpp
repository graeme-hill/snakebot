#include "onedirection.hpp"
#include "cautious.hpp"
#include "../movement.hpp"
#include "../astar.hpp"

#include <functional>
#include <unordered_map>
#include <sstream>

std::unordered_map<std::string, bool> OneDirection::_finishMap;

OneDirection::OneDirection(Direction direction) : _direction(direction)
{ }

Metadata OneDirection::meta()
{
    std::string left = "http://www.clker.com/cliparts/d/3/0/b/1206569743961899636pitr_green_arrows_set_4.svg.med.png";
    std::string right = "http://weclipart.com/gimg/6B83AEBB1C202B97/large-Arrow-Right-66.6-2393.png";
    std::string up = "http://www.clker.com/cliparts/e/v/b/X/Q/t/arrow-up-md.png";
    std::string down = "https://upload.wikimedia.org/wikipedia/commons/thumb/3/30/Gtk-go-down.svg/120px-Gtk-go-down.svg.png";

    std::string img = _direction == Direction::Left ? left
        : _direction == Direction::Right ? right
        : _direction == Direction::Up ? up
        : down;

    return {
        "#008888",
        "#FFFFFF",
        img,
        "OneDirection",
        "Yum",
        "pixel",
        "pixel"
    };
}

void OneDirection::start(std::string id)
{
    _finishMap[id] = false;
}

MaybeDirection tryMove(Direction direction, GameState &state)
{
    Point current = state.mySnake()->head();
    Point destination = coordAfterMove(current, direction);
    bool oob = outOfBounds(destination, state);
    if (!oob && state.map().turnsUntilVacant(destination) == 0)
    {
        return MaybeDirection::just(direction);
    }
    return MaybeDirection::none();
}

Direction OneDirection::move(GameState &state)
{
    return move(state, 0);
}

Direction OneDirection::move(GameState &state, uint32_t branchId)
{
    std::stringstream ss;
    ss << state.gameId() << "_" << branchId;
    std::string key = ss.str();

    // If already finished moving in this direction then switch to cautious.
    auto finishIter = _finishMap.find(key);
    if (finishIter != _finishMap.end() && finishIter->second)
    {
        Cautious cautious;
        return cautious.move(state);
    }

    // Should already be in the dictionary from call to start() but just make
    // sure in case server restarts or something.
    _finishMap[key] = false;

    // Try to move in specified direction.
    MaybeDirection attempt = tryMove(_direction, state);
    if (attempt.hasValue) return attempt.value;

    // Can't move in this direction anymore so change to backup algorithm.
    _finishMap[key] = true;
    Cautious cautious;
    return cautious.move(state);
}
