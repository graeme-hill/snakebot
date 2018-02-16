#pragma once

#include "snakelib.hpp"

enum class TerminationReason
{
    Loss, MaxTurns, OutOfTime, Unknown
};

struct AlgorithmPair
{
    Algorithm *myAlgorithm;
    Algorithm *enemyAlgorithm;
};

struct AlgorithmBranch
{
    AlgorithmPair pair;
    MaybeDirection firstMove;
};

struct Future
{
    std::unordered_map<std::string, uint32_t> obituaries;
    std::unordered_map<std::string, std::vector<uint32_t>> foodsEaten;
    Algorithm *algorithm;
    TerminationReason terminationReason;
    Direction move;
    uint32_t turns;
};

struct DirectionScore
{
    Direction direction;
    int score;
};

class Simulation
{
public:
    Simulation(
        AlgorithmBranch branch,
        GameState &initialState,
        uint32_t maxTurns,
        uint32_t simNumber);

    bool next();
    Future result() { return _result; }
    uint32_t simNumber() { return _simNumber; }

private:
    Direction getMyMove(GameState &state)
    {
        if (_turn == 1 && _branch.firstMove.hasValue)
        {
            return _branch.firstMove.value;
        }
        else
        {
            return _branch.pair.myAlgorithm->move(state);
        }
    }

    void updateObituaries(GameState &newState, GameState &oldState)
    {
        for (auto pair : oldState.snakes())
        {
            Snake *snake = pair.second;
            auto newIter = newState.snakes().find(snake->id);
            if (newIter == newState.snakes().end())
            {
                _result.obituaries[snake->id] = _turn;
            }
        }
    }

    void updateFoodsEaten(GameState &newState, GameState &oldState)
    {
        for (Point food : oldState.food())
        {
            Snake *inThatCellNow = newState.map().getSnake(food);
            if (inThatCellNow != nullptr)
            {
                auto iter = _result.foodsEaten.find(inThatCellNow->id);
                if (iter == _result.foodsEaten.end())
                {
                    _result.foodsEaten[inThatCellNow->id] = {};
                }
                _result.foodsEaten[inThatCellNow->id].push_back(_turn);
            }
        }
    }

    AlgorithmBranch _branch;
    GameState &_initialState;
    uint32_t _maxTurns;
    uint32_t _simNumber;
    uint32_t _turn;
    Future _result;
    std::unique_ptr<GameState> _newestState;
};

std::vector<Future> runSimulations(
    std::vector<AlgorithmPair> algorithmPairs,
    GameState &initialState,
    uint32_t maxTurns,
    std::vector<Direction> firstMoves);

std::vector<Future> simulateFutures(
    GameState &initialState,
    uint32_t maxTurns,
    std::vector<Algorithm *> myAlgorithms,
    std::vector<Algorithm *> enemyAlgorithms);

Direction bestMove(std::vector<Future> &futures, GameState &state);