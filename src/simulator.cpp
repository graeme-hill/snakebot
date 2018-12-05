#include "simulator.hpp"
#include "movement.hpp"
#include <cmath>
#include <sstream>
#include <numeric>

#define IDEAL_HEALTH_AT_FOOD_TIME 100

std::vector<std::unique_ptr<SimThread>> SimThread::instances;

AlgorithmPair PrefixedAlgorithmPair::unprefixed()
{
    return { myAlgorithm.algorithm, enemyAlgorithm.algorithm };
}

std::string fakeGameId()
{
    static uint32_t count = 0;
    count++;
    std::stringstream ss;
    ss << "sim_" << count;
    return ss.str();
}

SimThread::SimThread() :
    _hasWork(false),
    _quit(false),
    _thread(&SimThread::spin, this),
    _timeOfLastWork(Clock::now()),
    _sleeping(true)
{ }

void SimThread::stopAll()
{
    std::cout << "Waiting for simulation threads to stop...\n";

    for (std::unique_ptr<SimThread> &simThread : SimThread::instances)
    {
        simThread->kill();
    }

    for (std::unique_ptr<SimThread> &simThread : SimThread::instances)
    {
        simThread->join();
    }

    SimThread::instances.clear();
}

void SimThread::startAll()
{
    if (SimThread::instances.empty())
    {
        unsigned cores = std::thread::hardware_concurrency();
        std::cout << "Starting " << cores << " simulation threads" << std::endl;
        for (unsigned i = 0; i < cores; i++)
        {
            instances.push_back(std::make_unique<SimThread>());
        }
    }
}

void SimThread::wakeAll()
{
    // Call this just in case it hasn't already been called. It does nothing
    // after the first time.
    SimThread::startAll();

    for (std::unique_ptr<SimThread> &simThread : SimThread::instances)
    {
        simThread->wakeUp();
    }
}

std::vector<Future> &SimThread::result()
{
    return _result;
}

bool SimThread::done()
{
    return !_hasWork;
}

void SimThread::kill()
{
    _quit = true;
}

void SimThread::join()
{
    _thread.join();
}

void SimThread::wakeUp()
{
    _sleeping = false;
}

void SimThread::sleep()
{
    _sleeping = true;
}

void SimThread::startWork(SimParams params)
{
    _params = std::move(params);
    _hasWork = true;
}

void SimThread::spin()
{
    while (!_quit)
    {
        if (_hasWork)
        {
            if (_sleeping)
            {
                wakeUp();
            }

            _result = runSimulationBranches(
                _params.branches,
                *_params.state,
                _params.maxTurns,
                _params.maxMillis);
            _hasWork = false;
            _timeOfLastWork = Clock::now();
        }
        else if (!_sleeping)
        {
            auto now = Clock::now();
            Seconds diff = now - _timeOfLastWork;
            if (diff > Seconds(SECONDS_OF_NO_WORK_UNTIL_SLEEP))
            {
                sleep();
            }
        }

        if (_sleeping)
        {
            // Play very nice with other threads and take very little CPU
            std::this_thread::sleep_for(
                std::chrono::milliseconds(SLEEP_MODE_DELAY_MILLIS));
        }
        else
        {
            // Play nice with other threads but take a lot of CPU
            std::this_thread::yield();
        }
    }
}

std::string terminationReasonToString(TerminationReason reason)
{
    switch (reason)
    {
        case TerminationReason::Loss: return "Loss";
        case TerminationReason::MaxTurns: return "MaxTurns";
        case TerminationReason::OutOfTime: return "OutOfTime";
        default: return "Unknown";
    }
}

std::string prefixToString(std::vector<Direction> &prefix)
{
    std::stringstream ss;
    ss << "[";
    std::string sep = "";
    for (Direction d : prefix)
    {
        ss << sep << directionToString(d);
        sep = ", ";
    }
    ss << "]";
    return ss.str();
}

void Future::prettyPrint()
{
    std::cout << "Future: myAlgo=" << source.pair.myAlgorithm->meta().name
        << " turns=" << turns
        << " move=" << directionToString(move)
        << " prefix=" << prefixToString(source.firstMoves)
        << " termination=" << terminationReasonToString(terminationReason)
        << std::endl;

    if (!obituaries.empty())
    {
        std::cout << "  obituaries:";
        for (auto it : obituaries)
        {
            std::cout << " " << it.first << "=" << it.second;
        }
        std::cout << std::endl;
    }

    if (!foodsEaten.empty())
    {
        std::cout << "  food:";
        for (auto it : foodsEaten)
        {
            auto turns = it.second;
            if (!turns.empty())
            {
                std::cout << " " << it.first << "=[";
                std::string sep = "";
                for (auto t : turns)
                {
                    std::cout << sep << t;
                    sep = ",";
                }
                std::cout << "]";
            }
        }
        std::cout << std::endl;
    }
}

Simulation::Simulation(
    AlgorithmBranch branch,
    uint32_t branchId,
    GameState &initialState,
    uint32_t maxTurns,
    uint32_t maxMillis,
    uint32_t simNumber,
    AxisBias bias)
    :
    _branch(branch),
    _branchId(branchId),
    _initialState(initialState),
    _maxTurns(maxTurns),
    _maxMillis(maxMillis),
    _simNumber(simNumber),
    _enemyPathfindingBias(bias),
    _turn(0),
    _result(
        {{}, {}, TerminationReason::Unknown, Direction::Left, 0, branch})
{ }

bool Simulation::next()
{
    _turn++;
    GameState &currentState = _newestState ? *_newestState : _initialState;

    // My move.
    auto myMoveDir = getMyMove(currentState, _branchId);
    SnakeMove myMove = { currentState.mySnake(), myMoveDir };
    std::vector<SnakeMove> moves { myMove };
    if (_result.turns == 0)
    {
        _result.move = myMove.direction;
    }
    _result.turns++;

    // Enemy moves.
    for (Snake *enemy : currentState.enemies())
    {
        GameState &enemyState = currentState.perspective(
            enemy, _enemyPathfindingBias);

        // Add bug number to branch id so it won't conflict with my move's
        // branch id. It's a hack for when both use same algorithm and they
        // need to be treated as their own instances.
        Direction direction = _branch.pair.enemyAlgorithm->move(
            enemyState, _branchId + 100000);
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

std::vector<Future> runSimulationBranches(
    std::vector<AlgorithmBranch> &branches,
    GameState &initialState,
    uint32_t maxTurns,
    uint32_t maxMillis)
{
    auto start = Clock::now();
    auto maxSeconds = Seconds(static_cast<double>(maxMillis) / 1000.0);
    uint32_t turn = 0;
    std::vector<Simulation> simulations;
    uint32_t simIndex = 0;
    std::vector<Future> results;
    uint32_t nextId = 1;

    for (AlgorithmBranch &branch : branches)
    {
        AxisBias bias = branch.enemyPathBindingBias;
        simulations.push_back(
            { branch, nextId++, initialState, maxTurns, maxMillis, simIndex++, bias });

        results.push_back({
            {},
            {},
            TerminationReason::Unknown,
            Direction::Left,
            0,
            branch
        });
    }

    // Keep track of simulations that are finished so we know when to stop
    std::unordered_set<uint32_t> completedSimulations;

    bool outOfTime = false;

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

            auto now = Clock::now();
            Seconds diff = now - start;
            outOfTime = diff >= maxSeconds;

            if (sim.next() || turn >= maxTurns || outOfTime)
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

    // std::cout << "simulated " << turn << " turns | oot: " << outOfTime
    //     << " | branches: " << branches.size() << std::endl;

    return results;
}

std::vector<Future> runSimulations(
    std::vector<PrefixedAlgorithmPair> algorithmPairs,
    GameState &initialState,
    uint32_t maxTurns,
    uint32_t maxMillis)
{
    uint32_t branchIndex = 0;
    size_t threads = SimThread::instances.size();
    std::vector<std::vector<AlgorithmBranch>> branches(threads);
    for (PrefixedAlgorithmPair pair : algorithmPairs)
    {
        if (pair.myAlgorithm.prefixes.empty())
        {
            branches[branchIndex++ % threads].push_back(
                { pair.unprefixed(), { }, AxisBias::Horizontal });

            branches[branchIndex++ % threads].push_back(
                { pair.unprefixed(), { }, AxisBias::Vertical });
        }

        for (std::vector<Direction> &prefix : pair.myAlgorithm.prefixes)
        {
            branches[branchIndex++ % threads].push_back(
                { pair.unprefixed(), prefix, AxisBias::Horizontal });

            branches[branchIndex++ % threads].push_back(
                { pair.unprefixed(), prefix, AxisBias::Vertical });
            // MaybeDirection maybeDir { true, firstDir };
            //
            // uint32_t threadIndex1 = branchIndex++ % THREAD_COUNT;
            // AlgorithmBranch branch1{ pair, maybeDir, AxisBias::Horizontal };
            // branches[threadIndex1].push_back(branch1);
            //
            // uint32_t threadIndex2 = branchIndex++ % THREAD_COUNT;
            // AlgorithmBranch branch2{ pair, maybeDir, AxisBias::Vertical };
            // branches[threadIndex2].push_back(branch2);
        }
    }

    for (uint32_t g = 0; g < threads; g++)
    {
        SimThread::instances[g]->startWork(
            { branches[g], initialState.clone(), maxTurns, maxMillis });
    }

    bool anyIncomplete = true;
    while (anyIncomplete)
    {
        anyIncomplete = false;
        for (std::unique_ptr<SimThread> &simThread : SimThread::instances)
        {
            if (!simThread->done())
            {
                anyIncomplete = true;
            }
        }
        std::this_thread::yield();
    }

    std::vector<Future> result;
    for (std::unique_ptr<SimThread> &simThread : SimThread::instances)
    {
        std::vector<Future> thisResult = simThread->result();
        result.insert(result.end(), thisResult.begin(), thisResult.end());
    }

    return result;
}

std::vector<Future> simulateFutures(
    GameState &initialState,
    uint32_t maxTurns,
    uint32_t maxMillis,
    std::vector<PrefixedAlgorithm> myAlgorithms,
    std::vector<PrefixedAlgorithm> enemyAlgorithms)
{
    // Make algorithm pairs
    std::vector<PrefixedAlgorithmPair> algorithmPairs;
    for (PrefixedAlgorithm a1 : myAlgorithms)
    {
        for (PrefixedAlgorithm a2 : enemyAlgorithms)
        {
            algorithmPairs.push_back({ a1, a2 });
        }
    }

    // DirectionSet firstMoves = safeMoves(initialState);
    //
    // if (firstMoves.empty())
    // {
    //     firstMoves = riskyMoves(initialState);
    // }

    std::vector<Future> futures = runSimulations(
        algorithmPairs, initialState, maxTurns, maxMillis);

    return futures;
}

int getFoodScore(uint32_t foodTurn, GameState &state)
{
    int health = state.mySnake()->health;
    int healthAtFoodTime = health - foodTurn;
    int diff = healthAtFoodTime - IDEAL_HEALTH_AT_FOOD_TIME;
    int multiplier = 200;
    if (diff < 0)
    {
        multiplier = 100;
    }
    int inverseDiff = IDEAL_HEALTH_AT_FOOD_TIME - std::abs(diff);
    return inverseDiff * multiplier;
}

int scoreFuture(Future &future, GameState &state, MaybeDirection preferred)
{
    auto myId = state.mySnake()->id;
    uint32_t survivalScore = 1000000000;
    uint32_t murderScore = 0;
    uint32_t foodScore = 0;
    bool dies = false;
    bool isPreferredDirection =
        preferred.hasValue && preferred.value == future.move;
    uint32_t bonus = isPreferredDirection ? 500 : 0;
    uint32_t nextFood = 1000; // a big number that indicates starvation

    auto foodIt = future.foodsEaten.find(state.mySnake()->id);
    if (foodIt != future.foodsEaten.end())
    {
        std::vector<uint32_t> &foodTurns = foodIt->second;
        if (!foodTurns.empty())
        {
            nextFood = foodTurns.at(0);
            foodScore = getFoodScore(foodTurns.at(0), state);
            //foodScore += 100U - (std::min(100U, foodTurns.at(0)));
        }
    }

    if (nextFood > state.mySnake()->health)
    {
        survivalScore = std::min(survivalScore, state.mySnake()->health * 100);
        dies = true;
    }

    for (auto pair : future.obituaries)
    {
        if (pair.first == myId)
        {
            survivalScore = std::min(survivalScore, pair.second * 100);
            dies = true;
        }
        else
        {
            murderScore += (100U - (std::min(100U, pair.second))) * 10000;
        }
    }

    if (!dies)
    {
        uint32_t accessible = 0;

        if(future.move == Direction::Up)
        {
            accessible = state.getSpacesUp();
        } 
        else if(future.move == Direction::Down)
        {
            accessible = state.getSpacesDown();
        } 
        else if(future.move == Direction::Left)
        {
            accessible = state.getSpacesLeft();
        } 
        else 
        {
            accessible = state.getSpacesRight();
        }
        if (accessible < state.mySnake()->length())
        {
            std::cout << "TOO SMALL " << accessible << " | " << state.mySnake()->length()
                << " | " << directionToString(future.move) << "\n";
            survivalScore = std::min(survivalScore, accessible * 100U);
            dies = true;
        }
    }

    bool badCorner = couldEndUpCornerAdjacentToBiggerSnake(state, future.move);
    if (badCorner)
    {
        survivalScore = std::min(survivalScore, 1000U); // ???
        dies = true;
    }

    // auto foodIt = future.foodsEaten.find(state.mySnake()->id);
    // if (foodIt != future.foodsEaten.end())
    // {
    //     std::vector<uint32_t> &foodTurns = foodIt->second;
    //     if (!foodTurns.empty())
    //     {
    //         foodScore = getFoodScore(foodTurns.at(0), state);
    //         //foodScore += 100U - (std::min(100U, foodTurns.at(0)));
    //     }
    // }

    uint32_t finalScore = dies
        ? survivalScore + bonus
        : survivalScore + foodScore + murderScore + bonus;

    // std::cout << "survive: " << survivalScore
    //     << " murder: " << murderScore
    //     << " food: " << foodScore
    //     << " final: " << finalScore << std::endl;

    return finalScore;

    // auto obitIt = future.obituaries.find(state.mySnake()->id);
    // auto foodIt = future.foodsEaten.find(state.mySnake()->id);
    // uint32_t survivedTurns = obitIt == future.obituaries.end()
    //     ? future.turns
    //     : obitIt->second;
    // uint32_t survivalScore = log2(survivedTurns) * 1000;
    //
    // uint32_t foodPoints = 0;
    // if (foodIt != future.foodsEaten.end())
    // {
    //     std::vector<uint32_t> &foodTurns = foodIt->second;
    //     if (!foodTurns.empty())
    //     {
    //         foodPoints += foodScore(foodTurns.at(0), state);// 100 - foodTurns.at(0);
    //     }
    // }
    //
    // int score = survivalScore + foodPoints;
    // return score;
}

std::string directionScoreToString(DirectionScore ds)
{
    std::stringstream ss;
    ss << directionToString(ds.direction)
        << ", " << ds.score
        << ", " << ds.source->source.pair.myAlgorithm->meta().name;
    return ss.str();
}

std::string getKey(Future &f)
{
    std::stringstream ss;
    ss << f.source.pair.myAlgorithm->meta().name
        << ":" << prefixToString(f.source.firstMoves);
    return ss.str();
}

Direction bestMove(
    std::vector<Future> &futures, GameState &state, MaybeDirection preferred)
{
    // 1. Get worst score per first-algorithm, direction pair and store as
    //    score, direction pair (where direction is NOT unique).
    // 2. Take the pair with the best score and return its direction.

    // TODO: this key should be a struct with its own hash function, not string
    std::unordered_map<std::string, DirectionScore> worstScores;
    std::unordered_map<std::string, DirectionScore> bestScores;

    for (Future &future : futures)
    {
        //future.prettyPrint();

        // Should never have a zero move future, but if there is don't crash.
        if (future.turns == 0)
            continue;

        Direction direction = future.move;
        int score = scoreFuture(future, state, preferred);

        std::string key = getKey(future);

        //std::cout << "  " << key << "=" << score << std::endl;

        auto it = worstScores.find(key);
        if (it == worstScores.end())
        {
            worstScores[key] = { direction, score, &future };
        }
        else
        {
            DirectionScore existing = it->second;
            if (score < existing.score)
            {
                worstScores[key] = { direction, score, &future };
            }
        }

        it = bestScores.find(key);
        if (it == bestScores.end())
        {
            bestScores[key] = { direction, score, &future };
        }
        else
        {
            DirectionScore existing = it->second;
            if (score > existing.score)
            {
                bestScores[key] = { direction, score, &future };
            }
        }
    }

    DirectionScore bestOfTheWorst{ Direction::Up, -1, nullptr };
    DirectionScore bestOfTheBest{ Direction::Up, -1, nullptr };
    DirectionScore result{ Direction::Up, -1, nullptr };

    for (auto it : worstScores)
    {
        if (bestOfTheWorst.score < 0)
        {
            bestOfTheWorst = it.second;
        }
        else if (it.second.score > bestOfTheWorst.score)
        {
            bestOfTheWorst = it.second;
        }
    }

    for (auto it : bestScores)
    {
        if (bestOfTheBest.score < 0)
        {
            bestOfTheBest = it.second;
        }
        else if (it.second.score > bestOfTheBest.score)
        {
            bestOfTheBest = it.second;
        }
    }

    if (bestOfTheWorst.score < 1500)
    {
        std::cout << "TAKING MY CHANCES!!!\n";
        result = bestOfTheBest;
    }
    else
    {
        result = bestOfTheWorst;
    }

    std::cout << "Decision: " << directionScoreToString(result) << std::endl;

    return result.direction;
}
