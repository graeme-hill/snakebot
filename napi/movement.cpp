#include "movement.hpp"
#include "astar.hpp"
#include "snakelib.hpp"


bool is180(Point p, GameState &state)
{
    // If length of snake is 1 then nothing to worry about (... I think... might wanna test that)
    if (state.mySnake()->length() <= 1)
    {
        return false;
    }

    Point neck = state.mySnake()->parts.at(1);

    return neck == p;
}

bool isCellOk(Point p, GameState &state)
{
    return !outOfBounds(p, state)
        && state.map().turnsUntilVacant(p) == 0
        && !is180(p, state);
}

bool isCellSafe(Point p, GameState &state)
{
    return isCellOk(p, state) && !isCloseToEqualOrBiggerSnakeHead(p, state);
}

bool isCellRisky(Point p, GameState &state)
{
    return isCellOk(p, state) && isCloseToEqualOrBiggerSnakeHead(p, state);
}

MaybeDirection closestKillTunnelTarget(GameState &state)
{
    Snake *me = state.mySnake();
    Path best = Path::none();
    std::vector<Snake *> enemies = state.enemies();
    bool foundAnything = false;

    for (auto enemy : enemies)
    {
        Point head = enemy->head();
        Point currentCell = { head.x, head.y };
        std::vector<Point> cellPath;
        Point nextCell;

        int count = 0;
        //up
        if(isCellOk({currentCell.x, currentCell.y-1}, state)) 
        {
            nextCell = {currentCell.x, currentCell.y-1};
            count++;
        }

        //down
        if(isCellOk({currentCell.x, currentCell.y+1}, state)) 
        {
            nextCell = {currentCell.x, currentCell.y+1};
            count++;
        }

        //left
        if(isCellOk({currentCell.x-1, currentCell.y}, state)) 
        {
            nextCell = {currentCell.x-1, currentCell.y};
            count++;
        }

        //right
        if(isCellOk({currentCell.x+1, currentCell.y}, state)) 
        {
            nextCell = {currentCell.x+1, currentCell.y};
            count++;
        }

        //a kill tunnel can only exist if only one neighbour cell is open
        if(count != 1) {
            continue;
        }   

        int bailOut = 0;
        int maxLoop = state.width() * state.height();
        
        //counter here so this CAN'T go infinite
        Direction lastDirection;
        while (count == 1 && bailOut < maxLoop)
        {           

            count = 0;

            cellPath.push_back(nextCell);
            currentCell = nextCell;

            //up
            if(isCellOk({currentCell.x, currentCell.y-1}, state)) 
            {
                nextCell = {currentCell.x, currentCell.y-1};
                lastDirection = Direction::Up;
                count++;
            }

            //down
            if(isCellOk({currentCell.x, currentCell.y+1}, state)) 
            {
                nextCell = {currentCell.x, currentCell.y+1};
                lastDirection = Direction::Down;
                count++;
            }

            //left
            if(isCellOk({currentCell.x-1, currentCell.y}, state)) 
            {
                nextCell = {currentCell.x-1, currentCell.y};
                lastDirection = Direction::Left;
                count++;
            }

            //right
            if(isCellOk({currentCell.x+1, currentCell.y}, state)) 
            {
                nextCell = {currentCell.x+1, currentCell.y};
                lastDirection = Direction::Right;
                count++;
            }         

            bailOut++;    
        }

        if(cellPath.size() > 1) 
        {
            Point targetCell = coordAfterMove(cellPath.back(), lastDirection);
            std::cout << "TARGET CELL FOUND-->" << std::endl;
            targetCell.prettyPrint();
            Path myPath = shortestPath(me->head(), targetCell, state);
            if (!myPath.direction.hasValue)
            {
                continue;
            }

            if (myPath.size >= best.size && foundAnything)
            {
                continue;
            }

            foundAnything = true;
            best = myPath;
        }
    }

    return best.direction;
}

MaybeDirection closestFood(GameState &state)
{
    Snake *me = state.mySnake();
    Path best = Path::none();
    bool foundAnything = false;

    for (auto food : state.food())
    {
        Path myPath = shortestPath(me->head(), food, state);

        if (!myPath.direction.hasValue)
        {
            continue;
        }

        if (myPath.size >= best.size && foundAnything)
        {
            continue;
        }

        foundAnything = true;
        best = myPath;
    }

    return best.direction;
}

MaybeDirection bestFood(GameState &state)
{
    Snake *me = state.mySnake();
    Path best = Path::none();

    // Make copy of food vec sorted by distance to me. That way we can start
    // with the closest one.
    Point myHead = me->head();
    std::vector<Point> sortedFood = state.food();
    std::sort(sortedFood.begin(), sortedFood.end(),
        [myHead](Point a, Point b)
        {
            return distance(myHead, a) < distance(myHead, b);
        });

    for (Point food : sortedFood)
    {
        if (best.direction.hasValue)
        {
            auto myDirectDistance = distance(me->head(), food);
            if (myDirectDistance >= best.size)
            {
                // Since the foods are sorted by distance from me, and this one
                // is too far away to possibly beat the current best food, we
                // know that none of the next foods can beat it either.
                break;
            }
        }

        auto myPath = shortestPath(me->head(), food, state);

        if (!myPath.direction.hasValue)
            continue;

        if (best.direction.hasValue && myPath.size >= best.size)
            continue;

        bool enemyWillWin = false;

        // Copy enemies vec so that they can be sorted in-place without boneage.
        std::vector<Snake *> sortedEnemies = state.enemies();
        std::sort(sortedEnemies.begin(), sortedEnemies.end(),
            [food](Snake *a, Snake *b)
            {
                return distance(a->head(), food) < distance(b->head(), food);
            });

        for (Snake *enemy : sortedEnemies)
        {
            // If the shortest possible path with no barriers is longer than my
            // path then don't even bother with A* calculation. Furthermore,
            // since the snakes are already sorted by their distance from the
            // food, if this one is too far then so are all the rest.
            if (distance(food, enemy->head()) > myPath.size)
            {
                break;
            }

            // In this case it doesn't matter whether axis bias
            // is horizontal or vertical b/c we only care about
            // the size of the path, not the route it takes.
            GameState &enemyState = state.perspective(
                enemy, AxisBias::Horizontal);
            auto enemyPath = shortestPath(
                enemy->head(), food, enemyState);

            if (!enemyPath.direction.hasValue)
            {
                continue;
            }

            if (enemyPath.size == myPath.size)
            {
                enemyWillWin = me->length() < enemy->length();
            }
            else
            {
                enemyWillWin = enemyPath.size < myPath.size;
            }

            if (enemyWillWin)
            {
                break;
            }
        }

        if (!enemyWillWin)
        {
            best = myPath;
        }
    }

    return best.direction;
}

DirectionSet notImmediatelySuicidalMoves(GameState &state)
{
    Point myHead = state.mySnake()->head();
    uint32_t x = myHead.x;
    uint32_t y = myHead.y;
    DirectionSet moves;

    if (isCellOk({x - 1, y}, state))
    {
        moves.push(Direction::Left);
    }
    if (isCellOk({x + 1, y}, state))
    {
        moves.push(Direction::Right);
    }
    if (isCellOk({x, y - 1}, state))
    {
        moves.push(Direction::Up);
    }
    if (isCellOk({x, y + 1}, state))
    {
        moves.push(Direction::Down);
    }

    return moves;
}

DirectionSet safeMoves(GameState &state)
{
    Point myHead = state.mySnake()->head();
    uint32_t x = myHead.x;
    uint32_t y = myHead.y;
    DirectionSet moves;

    if (isCellSafe({x - 1, y}, state))
    {
        moves.push(Direction::Left);
    }
    if (isCellSafe({x + 1, y}, state))
    {
        moves.push(Direction::Right);
    }
    if (isCellSafe({x, y - 1}, state))
    {
        moves.push(Direction::Up);
    }
    if (isCellSafe({x, y + 1}, state))
    {
        moves.push(Direction::Down);
    }

    return moves;
}

DirectionSet riskyMoves(GameState &state)
{
    Point myHead = state.mySnake()->head();
    uint32_t x = myHead.x;
    uint32_t y = myHead.y;
    DirectionSet moves;

    if (isCellRisky({x - 1, y}, state))
    {
        moves.push(Direction::Left);
    }
    if (isCellRisky({x + 1, y}, state))
    {
        moves.push(Direction::Right);
    }
    if (isCellRisky({x, y - 1}, state))
    {
        moves.push(Direction::Up);
    }
    if (isCellRisky({x, y + 1}, state))
    {
        moves.push(Direction::Down);
    }

    return moves;
}

MaybeDirection notImmediatelySuicidal(GameState &state)
{
    DirectionSet moves = notImmediatelySuicidalMoves(state);
    if (moves.empty())
    {
        return MaybeDirection{ false, Direction::Left };
    }
    else
    {
        return MaybeDirection{ true, *moves.begin() };
    }
}

MaybeDirection chaseTail(GameState &state)
{
    Point myHead = state.mySnake()->head();
    Point myTail = state.mySnake()->tail();
    Path path = shortestPath(myHead, myTail, state);
    return path.direction;
}
