#include "astar.hpp"

#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#define VERY_HIGH_COST 1000
#define INFINITY_COST 1000000
#define MAX_ITERATIONS 10000

struct MemoryPool
{
    MemoryPool() :
        closedSet(20),
        openSet(20),
        cameFrom(20),
        gScore(20),
        fScore(20),
        turns(20)
    { }

    MemoryPool(const MemoryPool &) = delete;
    MemoryPool(MemoryPool &&) = delete;

    std::unordered_set<uint32_t> closedSet;
    std::unordered_set<uint32_t> openSet;
    std::unordered_map<uint32_t, uint32_t> cameFrom;
    std::unordered_map<uint32_t, uint32_t> gScore;
    std::unordered_map<uint32_t, uint32_t> fScore;
    std::unordered_map<uint32_t, uint32_t> turns;

    static thread_local MemoryPool instance;
};

thread_local MemoryPool MemoryPool::instance;

void clean(MemoryPool &pool)
{
    pool.closedSet.clear();
    pool.openSet.clear();
    pool.cameFrom.clear();
    pool.gScore.clear();
    pool.fScore.clear();
    pool.turns.clear();
}

inline uint32_t heuristicCostEstimate(Point start, Point goal)
{
    return distance(start, goal);
}

inline uint32_t lowestFScoreInSet(
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

inline Path reconstructPath(
    std::unordered_map<uint32_t, uint32_t> &cameFrom, uint32_t currentIndex, uint32_t width)
{
    MaybeDirection result = MaybeDirection::none();
    auto iter = cameFrom.find(currentIndex);
    size_t size = 0;
    while (iter != cameFrom.end())
    {
        uint32_t nextIndex = iter->second;
        Direction direction = directionBetweenNodes(nextIndex, currentIndex, width);
        result = MaybeDirection::just(direction);
        size++;
        currentIndex = nextIndex;
        iter = cameFrom.find(currentIndex);
    }

    return Path{ size, result };
}

inline bool indexIsSafe(uint32_t index, uint32_t turn, GameState &state)
{
    Point coord = deconstructCellIndex(index, state);
    if (outOfBounds(coord, state))
    {
        return false;
    }
    uint32_t turnsUntilCouldBeVacant = state.map().turnsUntilVacant(coord);
    return turnsUntilCouldBeVacant < turn;
}

inline bool is180(uint32_t index, uint32_t neighborIndex, GameState &state)
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

inline bool isOkNeighbor(
    uint32_t index, uint32_t other, uint32_t turn, GameState &state)
{
    bool result = indexIsSafe(other, turn, state)
        && !is180(index, other, state)
        && isAdjacent(index, other, state);

    return result;
}

inline std::vector<uint32_t> getNeighbors(
    uint32_t index, uint32_t turn, GameState &state)
{
    std::vector<uint32_t> result;

    uint32_t right = index + 1;
    uint32_t left = index - 1;
    uint32_t down = index + state.width();
    uint32_t up = index - state.width();

    if (state.pathfindingBias() == AxisBias::Vertical)
    {
        if (isOkNeighbor(index, up, turn, state)) result.push_back(up);
        if (isOkNeighbor(index, down, turn, state)) result.push_back(down);
        if (isOkNeighbor(index, left, turn, state)) result.push_back(left);
        if (isOkNeighbor(index, right, turn, state)) result.push_back(right);
    }
    else
    {
        if (isOkNeighbor(index, left, turn, state)) result.push_back(left);
        if (isOkNeighbor(index, right, turn, state)) result.push_back(right);
        if (isOkNeighbor(index, up, turn, state)) result.push_back(up);
        if (isOkNeighbor(index, down, turn, state)) result.push_back(down);
    }

    return result;
}

inline uint32_t getGScore(
    std::unordered_map<uint32_t, uint32_t> &gScore, uint32_t index)
{
    auto iter = gScore.find(index);
    if (iter == gScore.end())
    {
        return INFINITY_COST;
    }
    else
    {
        return iter->second;
    }
}

void printSet(std::string name, std::unordered_set<uint32_t> set)
{
    std::cout << name << ": ";
    for (auto i : set)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

void printMap(std::string name, std::unordered_map<uint32_t, uint32_t> map)
{
    std::cout << name << ": ";
    for (auto p : map)
    {
        std::cout << p.first << "->" << p.second << " ";
    }
    std::cout << std::endl;
}

Path shortestPath(Point start, Point goal, GameState &state)
{
    uint32_t safety = 0;

    uint32_t startIndex = cellIndex(start, state);
    uint32_t goalIndex = cellIndex(goal, state);

    bool isFirstMove = true;
    MemoryPool &pool = MemoryPool::instance; // reuse same pool of heap memory to avoid extra (de)allocations
    clean(pool);
    std::unordered_set<uint32_t> &closedSet = pool.closedSet;
    std::unordered_set<uint32_t> &openSet = pool.openSet;
    std::unordered_map<uint32_t, uint32_t> &cameFrom = pool.cameFrom;
    std::unordered_map<uint32_t, uint32_t> &gScore = pool.gScore;
    std::unordered_map<uint32_t, uint32_t> &fScore = pool.fScore;
    std::unordered_map<uint32_t, uint32_t> &turns = pool.turns;

    openSet.insert(startIndex);
    gScore.emplace(startIndex, 0);
    auto estimate = heuristicCostEstimate(start, goal);
    fScore.emplace(startIndex, estimate);
    turns.emplace(startIndex, 1);

    while (!openSet.empty())
    {
        // Make sure to never get stuck in loop.
        if (safety++ > MAX_ITERATIONS)
        {
            std::cout << "Out of control loop in A*! Quitting after max iterations." << std::endl;
            break;
        }

        uint32_t currentIndex = lowestFScoreInSet(openSet, fScore);
        if (currentIndex == goalIndex)
        {
            Path path = reconstructPath(cameFrom, currentIndex, state.width());
            return path;
        }

        openSet.erase(currentIndex);
        closedSet.insert(currentIndex);

        std::vector<uint32_t> neighbors = getNeighbors(
            currentIndex, turns[currentIndex], state);
        for (uint32_t neighborIndex : neighbors)
        {
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
    return Path::none();
}
