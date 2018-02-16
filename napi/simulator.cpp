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
        {{}, {}, branch.pair.myAlgorithm, TerminationReason::Unknown, Direction::Left, 0})
{ }

bool Simulation::next()
{
    _turn++;
    GameState &currentState = _newestState ? *_newestState : _initialState;

    // My move.
    SnakeMove myMove = { currentState.mySnake(), getMyMove(currentState) };
    std::vector<SnakeMove> moves { myMove };
    if (_result.turns == 0)
    {
        _result.move = myMove.direction;
    }
    _result.turns++;

    // Enemy moves.
    for (Snake *enemy : currentState.enemies())
    {
        GameState &enemyState = currentState.perspective(enemy);
        Direction direction = _branch.pair.enemyAlgorithm->move(enemyState);
        moves.push_back({ enemy, direction });
    }

    std::unique_ptr<GameState> newState = 
        currentState.newStateAfterMoves(moves);

    updateObituaries(*newState, currentState);
    updateFoodsEaten(*newState, currentState);

    _newestState = std::move(newState);

    if (_newestState->isLoss())
    {
        _result.terminationReason = TerminationReason::Loss;
        return true;
    }

    return false;
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
    DirectionSet firstMoves)
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
            MaybeDirection maybeDir { true, firstDir };
            AlgorithmBranch branch{ pair, maybeDir };
            simulations.push_back(
                { branch, initialState, maxTurns, simIndex++ });
            results.push_back({
                {},
                {},
                algorithmPairs[simIndex].myAlgorithm,
                TerminationReason::Unknown,
                Direction::Left,
                0
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

    for (size_t i = 0; i < results.size(); i++)
    {
        results[i] = simulations[i].result();
        results[i].terminationReason = coerceTerminationReason(
            results[i].terminationReason, turn, maxTurns);
    }

    //std::cout << "simulated " << turn << " turns" << std::endl;

    return results;
}

std::vector<Future> simulateFutures(
    GameState &initialState,
    uint32_t maxTurns,
    std::vector<Algorithm *> myAlgorithms,
    std::vector<Algorithm *> enemyAlgorithms)
{
    // Make algorithm pairs
    std::vector<AlgorithmPair> algorithmPairs;
    for (Algorithm *a1 : myAlgorithms)
    {
        for (Algorithm *a2 : enemyAlgorithms)
        {
            algorithmPairs.push_back({ a1, a2 });
        }
    }    

    DirectionSet firstMoves = safeMoves(initialState);

    if (firstMoves.empty())
    {
        firstMoves = riskyMoves(initialState);
    }

    std::vector<Future> futures = runSimulations(
        algorithmPairs, initialState, maxTurns, firstMoves);

    return futures;
}

int scoreFuture(Future &future, GameState &state)
{
    auto obitIt = future.obituaries.find(state.mySnake()->id);
    auto foodIt = future.foodsEaten.find(state.mySnake()->id);
    uint32_t survivedTurns = obitIt == future.obituaries.end()
        ? future.turns
        : obitIt->second;
    uint32_t survivalScore = survivedTurns * 1000;

    uint32_t foodScore = 0;
    if (foodIt != future.foodsEaten.end())
    {
        std::vector<uint32_t> foodTurns = foodIt->second;
        if (!foodTurns.empty())
        {
            foodScore += 100 - foodTurns.at(0);
        }
    }

    int score = survivalScore + foodScore;
    return score;
}

Direction bestMove(std::vector<Future> &futures, GameState &state)
{
    // 1. Get worst score per first-algorithm, direction pair and store as
    //    score, direction pair (where direction is NOT unique).
    // 2. Take the pair with the best score and return its direction.

    // TODO: this key should be a struct with its own hash function, not string
    std::unordered_map<std::string, DirectionScore> worstScores;

    for (Future &future : futures)
    {
        // Should never have a zero move future, but if there is don't crash.
        if (future.turns == 0)
            continue;

        Direction direction = future.move;
        int score = scoreFuture(future, state);

        std::string algoName = future.algorithm->meta().name;
        std::string key = algoName + "_" + directionToString(direction);
        auto it = worstScores.find(key);
        if (it == worstScores.end())
        {
            worstScores[key] = DirectionScore{ direction, score };
        }
        else
        {
            DirectionScore existing = it->second;
            if (score > existing.score)
            {
                worstScores[key] = DirectionScore{ direction, score };
            }
        }
    }

    DirectionScore best{ Direction::Up, -1 };

    for (auto it : worstScores)
    {
        if (best.score < 0)
        {
            best = it.second;
        }
        else if (it.second.score > best.score)
        {
            best = it.second;
        }
    }

    return best.direction;
}