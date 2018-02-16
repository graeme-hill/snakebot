#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <algorithm>

enum class Direction
{
    Up, Down, Left, Right
};

struct Point
{
    uint32_t x, y;

    void prettyPrint();
};

inline bool operator==(const Point &a, const Point &b)
{
    return a.x == b.x && a.y == b.y;
}

namespace std
{
    template <>
    class hash<Point>
    {
    public :
        size_t operator()(const Point &p) const
        {
            return hash<uint32_t>()(p.x) ^ hash<uint32_t>()(p.y);
        }
    };
};

struct MaybeDirection
{
    bool hasValue;
    Direction value;

    static MaybeDirection none()
    {
        return MaybeDirection{ false, Direction::Left };
    }

    static MaybeDirection just(Direction d)
    {
        return MaybeDirection{ true, d };
    }
};

struct Path
{
    size_t size;
    MaybeDirection direction;

    static Path none()
    {
        return Path{ 0, MaybeDirection::none() };
    }
};

class DirectionIterator
{
public:
    DirectionIterator(std::array<MaybeDirection, 4> &values, size_t index) :
        _values(values), _index(index)
    { }

    bool operator==(const DirectionIterator &other)
    {
        return other._index == _index && &other._values == &_values;
    }

    bool operator!=(const DirectionIterator &other)
    {
        return other._index != _index || &other._values != &_values;
    }

    DirectionIterator &operator++()
    {
        // Find the next direction where hasValue==true
        for (size_t i = _index + 1; i < _values.size(); i++)
        {
            if (_values[i].hasValue)
            {
                _index = i;
                return *this;
            }
        }

        // Couldn't find a direction so put it at the end()
        _index = _values.size();
        return *this;
    }

    Direction &operator*()
    {
        return _values[_index].value;
    }

private:
    std::array<MaybeDirection, 4> &_values;
    size_t _index;
};

class DirectionSet
{
public:
    DirectionSet() :
        _values({
            MaybeDirection{ false, Direction::Left },
            MaybeDirection{ false, Direction::Right },
            MaybeDirection{ false, Direction::Up },
            MaybeDirection{ false, Direction::Down }
        }),
        _beginIndex(_values.size())
    { }

    void push(Direction dir)
    {
        size_t thisIndex = dirIndex(dir);
        _values[thisIndex].hasValue = true;
        _beginIndex = std::min(_beginIndex, thisIndex);
    }

    DirectionIterator begin()
    {
        return DirectionIterator(_values, _beginIndex);
    }

    DirectionIterator end()
    {
        return DirectionIterator(_values, _values.size());
    }

    bool empty()
    {
        return _beginIndex >= _values.size();
    }

private:
    size_t dirIndex(Direction dir)
    {
        switch (dir)
        {
            case (Direction::Left): return 0;
            case (Direction::Right): return 1;
            case (Direction::Up): return 2;
            default: return 3;
        }
    }
    std::array<MaybeDirection, 4> _values;
    size_t _beginIndex;
};

struct Snake
{
    std::string id;
    uint32_t health;
    std::vector<Point> parts;
    bool dead;

    void prettyPrint();
    Point head();
    Point tail();
    uint32_t length() { return parts.size(); }
};

struct SnakeMove
{
    Snake *snake;
    Direction direction;
};

struct World
{
    std::vector<Point> food;
    std::vector<Snake> snakes;
    uint32_t width;
    uint32_t height;
    std::string you;

    void prettyPrint();
};

struct Metadata
{
    std::string color;
    std::string secondaryColor;
    std::string headUrl;
    std::string name;
    std::string taunt;
    std::string headType;
    std::string tailType;
};

class GameState;

class Algorithm
{
public:
    Algorithm();
    virtual Metadata meta() = 0;
    virtual Direction move(GameState &) = 0;
    uint32_t id() { return _id; }

private:
    uint32_t _id;
    static uint32_t nextId();
    static uint32_t _nextId;
};

class Cell
{
public:
    void vacate(Snake *snake, uint32_t turn);
    void decrementVacate();
    void resetVacate();
    uint32_t vacated() { return _vacated; }
    void setSnake(Snake *snake) { _snake = snake; }
    Snake *snake() { return _snake; }

private:
    uint32_t _vacated = 0;
    Snake *_snake = nullptr;
};

class Map
{
public:
    Map(GameState &gameState);

    // delete move and copy ctors for now to avoid accidental copies
    Map(const Map &) = delete;
    Map(Map &&) = delete;

    uint32_t turnsUntilVacant(Point p);
    Snake *getSnake(Point p);
    void update();

    void printVacateGrid();

private:
    void updateVacateTurnsForSnake(Snake *snake);

    GameState &_gameState;
    std::vector<Cell> _cells;
};

class GameState
{
public:
    GameState(World w);

    // delete move and copy ctors for now to avoid accidental copies
    GameState(const GameState &) = delete;
    GameState(GameState &&) = delete;

    uint32_t width() { return _width; }
    uint32_t height() { return _height; }
    World &world() { return _world; }
    std::unordered_map<std::string, Snake *> &snakes() { return _snakes; }
    std::vector<Snake *> &enemies() { return _enemies; }
    std::vector<Point> &food() { return _food; }
    Snake *mySnake();
    Map &map() { return _map; }

    GameState &perspective(Snake *enemy);
    std::unique_ptr<GameState> newStateAfterMoves(
        std::vector<SnakeMove> &moves);

    bool isLoss();

private:
    void removeSnake(Snake *snake);

    uint32_t _width;
    uint32_t _height;
    std::vector<Point> _food;
    std::unordered_map<std::string, Snake *> _snakes;
    std::unordered_map<std::string, std::unique_ptr<GameState>> _perspectiveCopies;
    std::vector<Snake *> _enemies;
    Snake *_mySnake;
    World _world;
    Map _map;
};

inline Point coordAfterMove(Point p, Direction dir)
{
    switch (dir)
    {
        case Direction::Up: return { p.x, p.y - 1 };
        case Direction::Down: return { p.x, p.y + 1 };
        case Direction::Left: return { p.x - 1, p.y };
        default: return { p.x + 1, p.y };
    }
}

inline uint32_t cellIndex(Point p, uint32_t width)
{
    return width * p.y + p.x;
}

inline uint32_t cellIndex(Point p, GameState &gameState)
{
    return cellIndex(p, gameState.width());
}

inline Point deconstructCellIndex(uint32_t index, uint32_t width)
{
    return { index % width, index / width };
}

inline Point deconstructCellIndex(uint32_t index, GameState &state)
{
    return deconstructCellIndex(index, state.width());
}

inline bool outOfBounds(Point p, uint32_t width)
{
    // Since x and y are unsigned so they wrap and become very large numbers.
    return p.x >= width || p.y >= width;
}

inline bool outOfBounds(Point p, GameState &gameState)
{
    return outOfBounds(p, gameState.width());
}

inline Direction directionBetweenNodes(uint32_t fromIndex, uint32_t toIndex, uint32_t width)
{
    auto from = deconstructCellIndex(fromIndex, width);
    auto to = deconstructCellIndex(toIndex, width);

    if (from.x < to.x) return Direction::Right;
    if (to.x < from.x) return Direction::Left;
    if (from.y < to.y) return Direction::Down;
    return Direction::Up;
}

inline uint32_t absDiff(uint32_t a, uint32_t b)
{
    return std::max(a, b) - std::min(a, b);
}

std::string directionToString(Direction direction);

void applyMoves(World &world, std::vector<SnakeMove> &moves);

bool isAdjacent(uint32_t aIndex, uint32_t bIndex, GameState &state);

bool isCloseToHead(uint32_t index, Snake *snake, GameState &state);

bool isCloseToEqualOrBiggerSnakeHead(Point p, GameState &state);

bool isCloseToEqualOrBiggerSnakeHead(uint32_t index, GameState &state);