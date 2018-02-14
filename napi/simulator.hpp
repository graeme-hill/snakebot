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
    std::vector<Direction> firstMoves;
};

struct Future
{
    uint32_t turnsSimulated;
    std::map<uint32_t, uint32_t> obituaries;
    std::map<uint32_t, std::vector<uint32_t>> foodsEaten;
    Algorithm *algorithm;
    TerminationReason terminationReason;
    std::vector<Direction> moves;
};

class Simulation
{
public:
    Simulation(
        AlgorithmBranch branch,
        GameState &initialState
        uint32_t maxTurns,
        uint32_t simNumber);

    bool next();
    Future result() { return _result; }
    uint32_t simNumber() { return _simNumber; }

private:
    Direction getMyMove();
    void updateObituaries(GameState *newState);
    void updateFoodsEaten(GameState *newState);

    AlgorithmBranch _branch;
    uint32_t _maxTurns;
    GameState &_initialState;
    uint32_t _simNumber;
    GameState *_currentState;
    uint32_t _turn;
    Future _result;
};

std::vector<Future> runSimulations(
    std::vector<AlgorithmPair> algorithmPairs,
    GameState &initialState,
    uint32_t maxTurns,
    std::vector<Direction> firstMoves);

std::vector<Future> simulateFutures(
    GameState &initialState,
    uint32_t maxTurns,
    std::vector<Algorithm *> algorithms);