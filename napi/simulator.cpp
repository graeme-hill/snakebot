#include "simulator.hpp"

#include <numeric>

Simulation::Simulation(
    std::vectorAlgorithmBranch branch,
    GameState &initialState
    uint32_t maxTurns,
    uint32_t simNumber)
    :
    _branch(branch),
    _initialState(initialState),
    _maxTurns(maxTurns),
    _simNumber(simNumber),
    _currentState(&_initialState),
    _turn(0),
    _result(
        {0, {}, {}, branch.pair.myAlgorithm, TerminationReason::Unknown, {}})
{ }

bool Simulation::next()
{
    _turn++;

    // My move.
    SnakeMove myMove = { _currentState->mySnake(), getMyMove() };
    std::vector<Direction> moves { myMove };
    _result.moves.push_back(myMove.direction);

    // Enemy moves.
    for (Snake *enemy : _currentState->enemies())
    {
        GameState &enemyState = _currentState->perspective(enemy);
        Direction direction = _branch.pair.enemyAlgorithm->move(enemyState);
        moves.push_back({ enemy, direction });
    }

    GameState *newState;
    currentState.newStateAfterMoves(moves, &newState);

    updateObituaries(newState);
    updateFoodsEaten(newState);

    _currentState = newState;

    if (_currentState->isLoss())
    {
        _result.terminationReason = TerminationReason::Loss;
        return true;
    }
}

Direction Simulation::getMyMove()
{
    if (turn <= _branch.firstMoves.size())
    {
        return _branch.firstMoves.at(turn - 1);
    }
    else
    {
        return _branch.pair.myAlgorithm.move(*_currentState);
    }
}

TerminationReason coerceTerminationReason(
    TerminationReason current, uint32_t turn, uint32_t maxTurns)
{
    if (current == TerminationReason::Unknown)
    {
        return turn >= maxTurns
            ? TerminationReason::MaxTurns
            : TerminationReason::OutOfTime;
    }
    return current;
}

std::vector<Future> runSimulations(
    std::vector<AlgorithmPair> algorithmPairs,
    GameState &initialState,
    uint32_t maxTurns,
    std::vector<Direction> firstMoves)
{
    uint32_t turn = 0;

    // Create a simulation for every algorithm pair + firstMove branch
    std::vector<Simulation> simulations;
    std::vector<Future> results;
    uint32_t simIndex = 0;
    for (AlgorithmPair pair : algorithmPairs)
    {
        for (Direction firstDir : firstMoves)
        {
            AlgorithmBranch branch{ pair, { firstDir } };
            simulations.push_back(
                { branch, initialState, maxTurns, simIndex++ });
            results.push_back({
                0,
                {},
                {},
                algorithmPairs[simIndex].myAlgorithm,
                TerminationReason::Unknown,
                {}
            });
        }
    }

    // Keep track of simulations that are finished so we know when to stop
    std::unordered_set<uint32_t> completedSimulations;

    while (completedSimulations.size() < simulations.size())
    {
        turn++;

        for (Simulation &sim : simulations)
        {
            // Don't do anything if this sim is already done
            auto completedIter = completedSimulations.find(sim.simNumber());
            if (completedIter != completedSimulations.end())
            {
                continue;
            }

            if (sim.next() || turn >= maxTurns)
            {
                results[sim.sumNumber()] = sim.result();
                completedSimulations.push_back(sim.simNumber());
            }
        }
    }

    for (Future &future : results)
    {
        future.TerminationReason = coerceTerminationReason(
            future.terminationReason, turn, maxTurns);
    }

    std::cout << "simulated " << turn << " turns" << std::endl;

    return results;
}

std::vector<Future> simulateFutures(
    GameState &initialState,
    uint32_t maxTurns,
    std::vector<Algorithm *> algorithms)
{
    // Make algorithm pairs
    std::vector<AlgorithmPair> algorithmPairs;
    for (Algorithm *a1 : algorithms)
    {
        for (Algorithm *a2 : algorithms)
        {
            algorithmPairs.push_back({ a1, a2 });
        }
    }    

    std::vector<Direction> possibleFirstMoves = notImmediatelySuicidalMoves(
        initialState);

    if (possibleFirstMoves.empty())
    {
        // gonna die
        return runSimulations(algorithmPairs, initialState, maxTurns, {});
    }

    std::vector<Future> futures;
    for (Direction firstMove : possibleFirstMoves)
    {
        std::vector<Future> futuresInThisDirection = runSimulations(
            algorithmPairs, initialState, maxTurns, { firstMove });
        for (Future future : futuresInThisDirection)
        {
            futures.push_back(future);
        }
    }

    return futures;
}