#include "onedirection.hpp"
#include "../movement.hpp"
#include "../astar.hpp"

#include <functional>
#include <unordered_map>

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
        "Hungry",
        "Yum",
        "pixel",
        "pixel"
    };
}

void OneDirection::start(std::string id)
{
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
    MaybeDirection attempt = tryMove(_direction, state);
    if (attempt.hasValue) return attempt.value;

    if (_direction == Direction::Left || _direction == Direction::Right)
    {
        attempt = tryMove(Direction::Up, state);
        if (attempt.hasValue) return attempt.value;

        attempt = tryMove(Direction::Down, state);
        if (attempt.hasValue) return attempt.value;

        return _direction == Direction::Left
            ? Direction::Right
            : Direction::Left;
    }
    else
    {
        attempt = tryMove(Direction::Left, state);
        if (attempt.hasValue) return attempt.value;

        attempt = tryMove(Direction::Right, state);
        if (attempt.hasValue) return attempt.value;

        return _direction == Direction::Up
            ? Direction::Down
            : Direction::Up;
    }
}
