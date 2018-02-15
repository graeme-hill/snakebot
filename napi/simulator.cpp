#include "simulator.hpp"
#include "movement.hpp"

#include <numeric>

Simulation::Simulation(
    AlgorithmBranch branch,
    GameState &initialState,
    uint32_t maxTurns,
    uint32_t simNumber)
    :
    _branch(branch),
    _initialState(initialState),
    _maxTurns(maxTurns),
    _simNumber(simNumber),
    _turn(0),
    _result(
        {0, {}, {}, branch.pair.myAlgorithm, TerminationReason::Unknown, {}})
{ }

bool Simulation::next()
{
    _turn++;
    GameState &currentState = _newestState ? *_newestState : _initialState;

    // My move.
    SnakeMove myMove = { currentState.mySnake(), getMyMove(currentState) };
    std::vector<SnakeMove> moves { myMove };
    _result.moves.push_back(myMove.direction);

    // Enemy moves.
    for (Snake *enemy : currentState.enemies())
    {
        GameState &enemyState = currentState.perspective(enemy);
        Direction direction = _branch.pair.enemyAlgorithm->move(enemyState);
        moves.push_back({ enemy, direction });
    }

    _newestState = currentState.newStateAfterMoves(moves);

    updateObituaries(*_newestState, currentState);
    updateFoodsEaten(*_newestState, currentState);

    if (_newestState->isLoss())
    {
        _result.terminationReason = TerminationReason::Loss;
        return true;
    }

    return false;
}

Direction Simulation::getMyMove(GameState &state)
{
    if (_turn <= _branch.firstMoves.size())
    {
        return _branch.firstMoves.at(_turn - 1);
    }
    else
    {
        return _branch.pair.myAlgorithm->move(state);
    }
}

void Simulation::updateObituaries(GameState &newState, GameState &oldState)
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

void Simulation::updateFoodsEaten(GameState &newState, GameState &oldState)
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
                results[sim.simNumber()] = sim.result();
                completedSimulations.insert(sim.simNumber());
            }
        }
    }

    for (Future &future : results)
    {
        future.terminationReason = coerceTerminationReason(
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