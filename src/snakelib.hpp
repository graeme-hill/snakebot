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

#define MAX_SNAKES 10

enum class AxisBias
{
    Vertical,
    Horizontal
};

// Container for an instance of T that adds a flag to track whether the
// value "exists". In reality the value always exists so there needs to
// be a default state (ie: parameter-less ctor).
template <typename T>
class Maybe
{
public:
    Maybe() : _hasValue(false)
    { }
    Maybe(T value) : _hasValue(true), _value(value)
    { }

    bool hasValue() { return _hasValue; }
    T &value() { return _value; }

    static Maybe<T> none()
    {
        return Maybe<T>();
    }

    static Maybe<T> just(T value)
    {
        return Maybe<T>(value);
    }
private:

    bool _hasValue;
    T _value;
};

// T = type of values in dictionary (which will be wrapped in Maybe<T>)
// N = maximum number of keys in dictionary
// Keys are always integers b/w 0 and N-1.
// Keys must be unique.
// Values are intended to be small and copy-friendly.
template <typename T, size_t N>
class ArrayDict
{
public:
    ArrayDict()
    {
        // Initialize everything as empty
        _data.fill(Maybe<T>());
    }

    void set(size_t i, T val)
    {
        if (i < N)
        {
            _data[i] = Maybe<T>::just(val);
        }
    }

    Maybe<T> get(size_t i)
    {
        if (i < N)
        {
            return _data[i];
        }
        else
        {
            return Maybe<T>::none();
        }
    }

    void remove(size_t i)
    {
        if (i < N)
        {
            _data[i] = Maybe<T>::none();
        }
    }
private:
    std::array<Maybe<T>, N> _data;
};

// struct Proximities
// {
//     std::array<
// };

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

struct MaybePoint
{
    bool hasValue;
    Point value;

    static MaybePoint none()
    {
        return MaybePoint{ false, { 0, 0 } };
    }

    static MaybePoint just(Point p)
    {
        return MaybePoint{ true, p };
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

// template <typename T>
// class Quad
// {
//
// }

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
    std::string id;

    uint32_t spacesUp = 0;
    uint32_t spacesDown = 0;
    uint32_t spacesLeft = 0;
    uint32_t spacesRight = 0;
    
    bool hasFoundSpacesUp = false;
    bool hasFoundSpacesDown = false;
    bool hasFoundSpacesLeft = false;
    bool hasFoundSpacesRight = false;

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
    virtual Direction move(GameState &state, uint32_t branchId);
    virtual void start(std::string id) = 0;
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


uint32_t countAccessibleCells(GameState &state, Point start);

uint32_t countAccessibleCellsAfterMove(
    GameState &state, Snake *snake, Direction move);

class GameState
{
public:
    GameState(World w, AxisBias bias = AxisBias::Vertical);

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
    AxisBias pathfindingBias() { return _pathfindingBias; }
    std::string gameId() { return _world.id; }

    GameState &perspective(Snake *enemy, AxisBias bias);
    std::unique_ptr<GameState> newStateAfterMoves(
        std::vector<SnakeMove> &moves);
    std::unique_ptr<GameState> clone();

    uint32_t getSpacesUp() { 
        if (_world.hasFoundSpacesUp) {
            return _world.spacesUp;
        } else {
            _world.spacesUp = countAccessibleCellsAfterMove(*this, mySnake(), Direction::Up);
            _world.hasFoundSpacesUp = true;
            return _world.spacesUp;
        }         
    }

    uint32_t getSpacesDown() { 
        if (_world.hasFoundSpacesDown) {
            return _world.spacesDown;
        } else {
            _world.spacesDown =countAccessibleCellsAfterMove(*this, mySnake(), Direction::Down);
            _world.hasFoundSpacesDown = true;
            return _world.spacesDown;
        }         
    }

    uint32_t getSpacesLeft() { 
        if (_world.hasFoundSpacesLeft) {
            return _world.spacesLeft;
        } else {
            _world.spacesLeft = countAccessibleCellsAfterMove(*this, mySnake(), Direction::Left);
            _world.hasFoundSpacesLeft = true;
            return _world.spacesLeft;
        }         
    }

    uint32_t getSpacesRight() { 
        if (_world.hasFoundSpacesRight) {
            return _world.spacesRight;
        } else {
            _world.spacesRight = countAccessibleCellsAfterMove(*this, mySnake(), Direction::Right);
            _world.hasFoundSpacesRight = true;
            return _world.spacesRight;
        }         
    }

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
    AxisBias _pathfindingBias;
};

inline Point coordAfterMove(Point p, Direction dir, int range = 1)
{
    switch (dir)
    {
        case Direction::Up: return { p.x, p.y - range };
        case Direction::Down: return { p.x, p.y + range };
        case Direction::Left: return { p.x - range, p.y };
        default: return { p.x + range, p.y };
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

inline bool outOfBounds(Point p, uint32_t width, uint32_t height)
{
    // Since x and y are unsigned they wrap and become very large numbers.
    return p.x >= width || p.y >= height;
}

inline bool outOfBounds(Point p, GameState &gameState)
{
    return outOfBounds(p, gameState.width(), gameState.height());
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

std::string axisBiasToString(AxisBias bias);

void applyMoves(World &world, std::vector<SnakeMove> &moves);

inline bool isAdjacent(uint32_t aIndex, uint32_t bIndex, GameState &state)
{
    Point a = deconstructCellIndex(aIndex, state);
    Point b = deconstructCellIndex(bIndex, state);

    bool horizontallyAdjacent = absDiff(a.x, b.x) == 1 && a.y == b.y;
    bool verticallyAdjacent = absDiff(a.y, b.y) == 1 && a.x == b.x;

    return horizontallyAdjacent || verticallyAdjacent;
}

inline bool isCloseToHead(uint32_t index, Snake *snake, GameState &state)
{
    uint32_t headIndex = cellIndex(snake->head(), state);
    return isAdjacent(index, headIndex, state);
}

inline bool isCloseToEqualOrBiggerSnakeHead(uint32_t index, GameState &state)
{
    auto enemies = state.enemies();
    for (Snake *otherSnake : enemies)
    {
        bool otherSnakeIsTooBigToEat = state.mySnake()->length() <= otherSnake->length();
        if (otherSnakeIsTooBigToEat && isCloseToHead(index, otherSnake, state))
        {
            return true;
        }
    }
    return false;
}

inline bool isCloseToEqualOrBiggerSnakeHead(Point p, GameState &state)
{
    return isCloseToEqualOrBiggerSnakeHead(cellIndex(p, state), state);
}

inline uint32_t distance(Point a, Point b)
{
    return absDiff(a.x, b.x) + absDiff(a.y, b.y);
}

inline bool cellIsEmpty(GameState &state, Point cell)
{
    return state.map().turnsUntilVacant(cell) == 0;
}

bool couldEndUpCornerAdjacentToBiggerSnake(GameState &state, Direction direction);