#include "snakelib.hpp"

void Point::prettyPrint()
{
    std::cout << x << "," << y;
}

void Snake::prettyPrint()
{
    std::cout << id;
    if (health < 10)
         std::cout << "   " << health;
    else if (health < 100)
        std::cout << "  " << health;
    else
        std::cout << " " << health;
    std::cout << "hp <<";
    for (auto p : parts)
    {
        std::cout << "(";
        p.prettyPrint();
        std::cout << ")";
    }
    std::cout << "<<";
}

Point Snake::head()
{
    return parts.at(0);
}

Point Snake::tail()
{
    return parts.at(parts.size() - 1);
}

void World::prettyPrint()
{
    std::cout << "width: " << width << std::endl;
    std::cout << "height: " << height << std::endl;
    std::cout << "you: " << you << std::endl;
    std::cout << "snakes: (" << snakes.size() << ")" << std::endl;
    for (auto snake : snakes)
    {
        std::cout << "    ";
        snake.prettyPrint();
        std::cout << std::endl;
    }
    std::cout << "food: (" << food.size() << ")" << std::endl;
    for (auto point : food)
    {
        std::cout << "    ";
        point.prettyPrint();
        std::cout << std::endl;
    }
}

GameState::GameState(World w) :
    _width(w.width),
    _height(w.height),
    _food(w.food),
    _mySnake(nullptr),
    _world(w),
    _map(*this)
{
    for (size_t i = 0; i < _world.snakes.size(); i++)
    {
        Snake *snake = &_world.snakes[i];
        _snakes[snake->id] = snake;
        if (snake->id == _world.you)
        {
            _mySnake = snake;
        }
        else
        {
            _enemies.push_back(snake);
        }
    }

    _map.update();
}

Snake *GameState::mySnake()
{
    return _mySnake;
}

void Cell::vacate(Snake *snake, uint32_t turn)
{
    _snake = snake;
    if (turn > _vacated)
    {
        _vacated = turn;
    }
}

void Cell::decrementVacate()
{
    if (_vacated > 0)
    {
        _vacated--;
    }
}

void Cell::resetVacate()
{
    _vacated = 0;
}

Map::Map(GameState &gameState) :
    _gameState(gameState),
    _cells(gameState.width() * gameState.height())
{
    update();
}

uint32_t Map::turnsUntilVacant(Point p)
{
    return outOfBounds(p, _gameState)
        ? 0
        : _cells.at(cellIndex(p, _gameState)).vacated();
}

void Map::update()
{
    for (auto &pair : _gameState.snakes())
    {
        updateVacateTurnsForSnake(pair.second);
    }
}

void Map::updateVacateTurnsForSnake(Snake *snake)
{
    for (uint32_t i = 0; i < snake->parts.size(); i++)
    {
        Point p = snake->parts.at(i);
        uint32_t vacated = snake->parts.size() - i - 1;
        uint32_t index = cellIndex(p, _gameState);
        _cells[index].vacate(snake, vacated);
    }
}

std::string directionToString(Direction direction)
{
    switch (direction)
    {
        case Direction::Up: return "up";
        case Direction::Down: return "down";
        case Direction::Left: return "left";
        default: return "right";
    }
}