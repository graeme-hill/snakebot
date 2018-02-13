#include "astar.hpp"

#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#define VERY_HIGH_COST 1000

uint32_t heuristicCostEstimate(Point start, Point goal)
{
    return absDiff(start.x, goal.x) + absDiff(start.y, goal.y);
}

uint32_t lowestFScoreInSet(
    std::unordered_set<uint32_t> &set, std::unordered_map<uint32_t, uint32_t> &fScore)
{
    uint32_t lowest = 0;
    uint32_t lowestIndex = 0;
    bool first = true;

    for (auto index : set)
    {
        auto iter = fScore.find(index);
        if (iter != fScore.end())
        {
            uint32_t score = iter->second;
            if (first || score < lowest)
            {
                lowest = score;
                lowestIndex = index;
                first = false;
            }
        }
    }

    return lowestIndex;
}

std::vector<Direction> reconstructPath(
    std::unordered_map<uint32_t, uint32_t> &cameFrom, uint32_t currentIndex, uint32_t width)
{
    std::vector<Direction> moves;
    auto iter = cameFrom.find(currentIndex);
    while (iter != cameFrom.end())
    {
        uint32_t nextIndex = iter->second;
        Direction direction = directionBetweenNodes(nextIndex, currentIndex, width);
        moves.insert(moves.begin(), direction);
        currentIndex = nextIndex;
        iter = cameFrom.find(currentIndex);
    }

    return moves;
}

bool indexIsSafe(uint32_t index, uint32_t turn, GameState &state)
{
    Point coord = deconstructCellIndex(index, state);
    if (outOfBounds(coord, state))
    {
        return false;
    }
    uint32_t turnsUntilCouldBeVacant = state.map().turnsUntilVacant(coord);
    return turnsUntilCouldBeVacant < turn;
}

bool isAdjacent(uint32_t aIndex, uint32_t bIndex, GameState &state)
{
    Point a = deconstructCellIndex(aIndex, state);
    Point b = deconstructCellIndex(bIndex, state);

    bool horizontallyAdjacent = absDiff(a.x, b.x) == 1 && a.y == b.y;
    bool verticallyAdjacent = absDiff(a.y, b.y) == 1 && a.x == b.x;

    return horizontallyAdjacent || verticallyAdjacent;
}

bool is180(uint32_t index, uint32_t neighborIndex, GameState &state)
{
    // If length of snake is 1 then nothing to worry about (... I think... might wanna test that)
    if (state.mySnake()->length() <= 1)
    {
        return false;
    }

    Point head = state.mySnake()->parts.at(0);
    Point neck = state.mySnake()->parts.at(1);

    uint32_t headIndex = cellIndex(head, state);
    uint32_t neckIndex = cellIndex(neck, state);

    return index == headIndex && neighborIndex == neckIndex;
}

bool isCloseToHead(uint32_t index, Snake *snake, GameState &state)
{
    uint32_t headIndex = cellIndex(snake->head(), state);
    return isAdjacent(index, headIndex, state);
}

bool isCloseToEqualOrBiggerSnakeHead(uint32_t index, GameState &state)
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

bool isOkNeighbor(uint32_t index, uint32_t other, uint32_t turn, GameState &state)
{
    bool result = indexIsSafe(other, turn, state)
        && !is180(index, other, state)
        && isAdjacent(index, other, state);

    return result;
}

std::vector<uint32_t> getNeighbors(uint32_t index, uint32_t turn, GameState &state)
{
    std::vector<uint32_t> result;

    uint32_t right = index + 1;
    uint32_t left = index - 1;
    uint32_t down = index + state.width();
    uint32_t up = index - state.width();

    if (isOkNeighbor(index, up, turn, state)) result.push_back(up);
    if (isOkNeighbor(index, down, turn, state)) result.push_back(down);
    if (isOkNeighbor(index, left, turn, state)) result.push_back(left);
    if (isOkNeighbor(index, right, turn, state)) result.push_back(right);

    return result;
}

uint32_t getGScore(std::unordered_map<uint32_t, uint32_t> &gScore, uint32_t index)
{
    auto iter = gScore.find(index);
    if (iter == gScore.end())
    {
        return VERY_HIGH_COST;
    }
    else
    {
        return iter->second;
    }
}

std::vector<Direction> shortestPath(Point start, Point goal, GameState &state)
{
    std::cout << "***2.1.1\n";

    uint32_t startIndex = cellIndex(start, state);
    uint32_t goalIndex = cellIndex(goal, state);

    bool isFirstMove = false;
    std::unordered_set<uint32_t> closedSet;
    std::unordered_set<uint32_t> openSet;
    std::unordered_map<uint32_t, uint32_t> cameFrom;
    std::unordered_map<uint32_t, uint32_t> gScore;
    std::unordered_map<uint32_t, uint32_t> fScore;
    std::unordered_map<uint32_t, uint32_t> turns;

    std::cout << "***2.1.2\n";

    openSet.insert(startIndex);
    std::cout << "***2.1.2.1\n";
    gScore.emplace(startIndex, 0);
    std::cout << "***2.1.2.2\n";
    auto estimate = heuristicCostEstimate(start, goal);
    fScore.emplace(startIndex, estimate);
    std::cout << "***2.1.2.3\n";
    turns.emplace(startIndex, 1);

    std::cout << "***2.1.3\n";

    while (!openSet.empty())
    {
        std::cout << "***2.1.4\n";
        uint32_t currentIndex = lowestFScoreInSet(openSet, fScore);
        std::cout << "***2.1.5\n";
        if (currentIndex == goalIndex)
        {
            std::vector<Direction> path = reconstructPath(cameFrom, currentIndex, state.width());
            return path;
        }

        openSet.erase(currentIndex);
        closedSet.insert(currentIndex);

        std::vector<uint32_t> neighbors = getNeighbors(currentIndex, turns[currentIndex], state);
        for (uint32_t neighborIndex : neighbors)
        {
            std::cout << "***2.1.6\n";
            if (closedSet.find(neighborIndex) != closedSet.end())
            {
                continue;
            }

            if (openSet.find(neighborIndex) == openSet.end())
            {
                openSet.insert(neighborIndex);
            }

            uint32_t dontGetEatenModifier = 0;
            if (isFirstMove)
            {
                bool couldGetEaten = isCloseToEqualOrBiggerSnakeHead(neighborIndex, state);
                if (couldGetEaten)
                {
                    dontGetEatenModifier = VERY_HIGH_COST;
                }
            }

            uint32_t tentativeGScore = getGScore(gScore, currentIndex) + 1 + dontGetEatenModifier;

            if (tentativeGScore >= getGScore(gScore, neighborIndex))
            {
                continue;
            }

            cameFrom[neighborIndex] = currentIndex;
            gScore[neighborIndex] = tentativeGScore;
            turns[neighborIndex] = turns[currentIndex] + 1;
            fScore[neighborIndex] = gScore[neighborIndex] + heuristicCostEstimate(
                deconstructCellIndex(neighborIndex, state), goal);
        }

        isFirstMove = false;
    }

    // Found no path
    return std::vector<Direction>();
}