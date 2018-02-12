#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <array>
#include <unordered_map>
#include <memory>

enum class Direction
{
    Up, Down, Left, Right
};

struct Point
{
    uint32_t x, y;

    void prettyPrint();
};

    // uint32_t length() const { return parts.length; }
    // Point tail() const { return parts.at(parts.size() - 1); }
    // Point head() const { return parts.at(0); }

struct World
{
    struct Snake
    {
        std::string id;
        uint32_t health;
        std::vector<Point> parts;

        void prettyPrint();
    };

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

class Algorithm
{
public:
    virtual Metadata meta() = 0;
    virtual Direction move(World) = 0;
};

Point coordAfterMove(Point p, Direction dir)
{
    switch (dir)
    {
        case Direction::Up: return { p.x, p.y - 1 };
        case Direction::Down: return { p.x, p.y + 1 };
        case Direction::Left: return { p.x - 1, p.y };
        default: return { p.x + 1, p.y };
    }
}

class Snake;

class SnakePart
{
public:
    SnakePart(Point p) : _point(p), _snake(nullptr) { }
    Point point() { return _point; }
    Snake *snake() { return _snake; }
    void setSnake(Snake *s) { _snake = s; }

private:
    Point _point;
    Snake *_snake;
};

class Snake
{
public:
    Snake(std::string id) : _id(id) { }
    void addPart(SnakePart part);
    size_t length() const { return _parts.size(); }
    Point &tail() { return _parts.at(_parts.size() - 1); }
    Point &head() { return _parts.at(0); }
    std::string id() { return _id; }

private:
    std::string _id;
    std::vector<SnakePart> _parts;
};

class Map;

class GameState
{
public:
    GameState(World w);
    uint32_t width() { return _width; }
    uint32_t height() { return _height; }
    World &world() { return _world; }
    std::vector<Snake> &snakes() { return _snakes; }
    Snake *mySnake();

private:
    uint32_t _width;
    uint32_t _height;
    std::vector<Point> _food;
    std::vector<Snake> _snakes;
    std::string _myId;
    World _world;
    Map _map;
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

};