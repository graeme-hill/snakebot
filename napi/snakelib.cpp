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

void Snake::addPart(SnakePart part)
{
    part.setSnake(this);
    _parts.push_back(part);
}

GameState::GameState(World w) :
    _width(w.width),
    _height(w.height),
    _food(w.food),
    _mySnake(nullptr)
    _world(w)
{
    for (auto &snake : w.snakes)
    {
        _snakes[snake.id] = &snake;
        if (snake.id == _world.you)
        {
            _mySnake = &snake;
        }
        else
        {
            _enemies.push_back(&snake);
        }
    }

    _map.update();
}

Snake *GameState::mySnake()
{
    for (auto &snake : _snakes)
    {
        if (snake.id() == _myId)
        {
            return &snake;
        }
    }

    return nullptr;
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
    updateVacateTurns();
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

void updateVacateTurnsForSnake(Snake &snake)
{
    for (uint32_t i = 0; i < snake.parts.size(); i++)
    {
        Point p = snake.parts.at(i);
        uint32_t vacated = snake.parts.size() - i - 1;
        uint32_t index = cellIndex(p, _gameState);
        _cells[index].vacate(vacated);
    }
}