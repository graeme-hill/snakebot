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

uint32_t Algorithm::_nextId = 0;

Algorithm::Algorithm() : _id(nextId())
{ }

uint32_t Algorithm::nextId()
{
    return _nextId++;
}

Direction Algorithm::move(GameState &state, uint32_t branchId)
{
    return move(state);
}

GameState::GameState(World w, AxisBias bias) :
    _width(w.width),
    _height(w.height),
    _food(w.food),
    _mySnake(nullptr),
    _world(w),
    _map(*this),
    _pathfindingBias(bias)
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

GameState &GameState::perspective(Snake *enemy, AxisBias bias)
{
    auto iter = _perspectiveCopies.find(enemy->id);
    if (iter != _perspectiveCopies.end())
    {
        return *iter->second;
    }

    World newWorld = _world;
    newWorld.you = enemy->id;
    _perspectiveCopies.insert(
        std::make_pair(enemy->id,
            std::unique_ptr<GameState>(new GameState(newWorld, bias))));
    return *_perspectiveCopies[enemy->id];
}

std::unique_ptr<GameState> GameState::newStateAfterMoves(
    std::vector<SnakeMove> &moves)
{
    World newWorld = _world;
    applyMoves(newWorld, moves);
    return std::unique_ptr<GameState>(new GameState(newWorld));
}

std::unique_ptr<GameState> GameState::clone()
{
    World newWorld = _world;
    return std::unique_ptr<GameState>(new GameState(newWorld));
}

Snake *GameState::mySnake()
{
    return _mySnake;
}

bool GameState::isLoss()
{
    return _mySnake == nullptr;
}

void Cell::vacate(Snake *snake, uint32_t turn)
{
    if (turn > 0)
    {
        _snake = snake;
    }
    else
    {
        _snake = nullptr;
    }

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

    if (_vacated == 0)
    {
        _snake = nullptr;
    }
}

void Cell::resetVacate()
{
    _vacated = 0;
    _snake = nullptr;
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

Snake *Map::getSnake(Point p)
{
    return outOfBounds(p, _gameState)
        ? nullptr
        : _cells.at(cellIndex(p, _gameState)).snake();
}

void Map::printVacateGrid()
{
    std::cout << "--- vacate grid ---" << std::endl;
    for (uint32_t row = 0; row < _gameState.height(); row++)
    {
        for (uint32_t col = 0; col < _gameState.width(); col++)
        {
            uint32_t index = cellIndex({ col, row }, _gameState);
            std::cout << _cells[index].vacated() << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-------------------" << std::endl;
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

std::string axisBiasToString(AxisBias bias)
{
    if (bias == AxisBias::Vertical)
    {
        return "Vertical";
    }
    else
    {
        return "Horizontal";
    }
}

void moveHeadsForward(World &world, std::vector<SnakeMove> &moves)
{
    for (Snake &snake : world.snakes)
    {
        auto iter = std::find_if(
            moves.begin(), moves.end(), [&snake](const SnakeMove &sm)
            {
                return sm.snake->id == snake.id;
            });

        // For some reason this snake doesn't exist in the world obj. That's
        // weird but whatever.
        if (iter == moves.end())
            continue;

        Direction direction = (*iter).direction;
        Point destination = coordAfterMove(snake.head(), direction);
        snake.parts.insert(snake.parts.begin(), destination);
    }
}

void eatFoodOrDie(World &world)
{
    std::vector<Snake *> justAte;

    for (Point food : world.food)
    {
        Snake *bestEater = nullptr;

        for (Snake &snake : world.snakes)
        {
            if (snake.head() == food)
            {
                if (bestEater == nullptr)
                {
                    bestEater = &snake;
                }
                else if (bestEater->length() > snake.length())
                {
                    snake.dead = true;
                }
                else if (bestEater->length() == snake.length())
                {
                    snake.dead = true;
                    bestEater->dead = true;
                }
                else
                {
                    bestEater->dead = true;
                    bestEater = &snake;
                }
            }
        }

        if (bestEater != nullptr && !bestEater->dead)
        {
            justAte.push_back(bestEater);
        }
    }

    // TO DO: The definition of "just ate" actually needs to be based on last
    // turn because the tail actually moves forward as the snakes eats the food
    // and then stays in place on the following turn... will need to maintain
    // state across turns :(

    // Shorten tail for any snake that didn't just eat
    for (Snake &snake : world.snakes)
    {
        auto justAteIter = std::find(justAte.begin(), justAte.end(), &snake);
        if (justAteIter == justAte.end())
        {
            // It didn't eat so remove end of tail.
            snake.parts.pop_back();
        }
        else
        {
            // This snake just ate which means it needs to grow on the NEXT
            // turn. Implement by moving current tail piece forward once to
            // overlap new tail position. In this case length should always be
            // at least two because it just ate, but at least make sure not to
            // crash if that does somehow happen.
            if (snake.parts.size() >= 2)
            {
                size_t oldTailIndex = snake.parts.size() - 1;
                size_t newTailIndex = snake.parts.size() - 2;
                snake.parts[oldTailIndex] = snake.parts.at(newTailIndex);
            }

            //snake.parts.pop_back();
            // It did eat so remove the food it ate.
            world.food.erase(
                std::remove(
                    world.food.begin(),
                    world.food.end(),
                    snake.head()),
                world.food.end());
        }
    }
}

void markCrashersDead(World &world)
{
    std::unordered_map<Point, Snake *> tailCells;
    std::unordered_map<Point, Snake *> headCells;

    // Populate tailCells and headCells and handle head on collisions.
    for (Snake &snake : world.snakes)
    {
        // Head
        auto it = headCells.find(snake.head());
        if (it != headCells.end())
        {
            Snake *other = it->second;
            if (snake.length() < other->length())
            {
                // Get eaten by other snake.
                snake.dead = true;
            }
            else if (snake.length() == other->length())
            {
                // Neither can eat the other so both die.
                snake.dead = true;
                other->dead = true;
            }
            else
            {
                // Eat the other snake.
                other->dead = true;
                headCells[snake.head()] = &snake;
            }
        }
        else
        {
            // Only one in this cell so far.
            headCells[snake.head()] = &snake;
        }

        // Tail
        for (uint32_t i = 1; i < snake.length(); i++)
        {
            tailCells[snake.parts[i]] = &snake;
        }
    }

    // Find heads that ran into tails or went oob.
    for (Snake &snake : world.snakes)
    {
        if (outOfBounds(snake.head(), world.width, world.height))
        {
            snake.dead = true;
        }
        else
        {
            auto it = tailCells.find(snake.head());
            if (it != tailCells.end())
            {
                // Crashed into someone's tail (possible its own).
                snake.dead = true;
            }
        }
    }
}

void removeDeadGuys(World &world)
{
    world.snakes.erase(
        std::remove_if(world.snakes.begin(), world.snakes.end(),
            [](const Snake &s) { return s.dead; }),
        world.snakes.end());
}

void applyMoves(World &world, std::vector<SnakeMove> &moves)
{
    // Adds new part in direction of move. Does not handle collions, oob, etc.
    moveHeadsForward(world, moves);

    // Removes tail part if didn't eat. Marks dead if got eaten.
    eatFoodOrDie(world);

    // Finds deaths that ocurred on non-food cells.
    markCrashersDead(world);

    // Removes dead snakes from the board.
    removeDeadGuys(world);
}
