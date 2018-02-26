#include "sim.hpp"
#include "hungry.hpp"
#include "cautious.hpp"
#include "inyourface.hpp"
#include "terminator.hpp"
#include "random.hpp"
#include "dog.hpp"
#include "../movement.hpp"
#include "../astar.hpp"
#include "../simulator.hpp"

#include <functional>
#include <unordered_map>

Sim::Sim() : _maxTurns(10000), _maxMillis(100)
{ }

Sim::Sim(uint32_t maxTurns, uint32_t maxMillis) :
    _maxTurns(maxTurns), _maxMillis(maxMillis)
{ }

Metadata Sim::meta()
{
    return {
        "#FFFFFF",
        "#FFFFFF",
        "http://www.theglamnationnetwork.com/uploads/1/3/1/2/13123626/6958010.jpg",
        "White Snake",
        "Hi",
        "shades",
        "pixel"
    };
}

void Sim::start()
{
    // Make sure the simulation threads are all awake so that it's not slow on
    // the first move.
    SimThread::wakeAll();
}

Direction Sim::move(GameState &state)
{
    std::vector<std::vector<Direction>> myPrefixMoves {
        { Direction::Up, Direction::Up },
        { Direction::Up, Direction::Left },
        { Direction::Up, Direction::Right },
        { Direction::Down, Direction::Down },
        { Direction::Down, Direction::Left },
        { Direction::Down, Direction::Right },
        { Direction::Left, Direction::Left },
        { Direction::Left, Direction::Up },
        { Direction::Left, Direction::Down },
        { Direction::Right, Direction::Right },
        { Direction::Right, Direction::Up },
        { Direction::Right, Direction::Down }
    };

    std::vector<std::vector<Direction>> enemyPrefixMoves {};
    InYourFace inYourFace;
    InYourFace inMyFace(state.mySnake());
    Hungry hungry;
    Cautious cautious;
    Terminator terminator;
    Random random;
    Dog dog;

    std::vector<PrefixedAlgorithm> myAlgorithms {
        { &dog, myPrefixMoves },
        { &cautious, {} },
        { &inYourFace, {} },
    };
    std::vector<PrefixedAlgorithm> enemyAlgorithms {
        { &hungry, enemyPrefixMoves },
        { &inMyFace, enemyPrefixMoves }
    };

    Direction preferred = dog.move(state);

    std::vector<Future> futures = simulateFutures(
    	state, _maxTurns, _maxMillis, myAlgorithms, enemyAlgorithms);

    Direction best = bestMove(futures, state, MaybeDirection::just(preferred));

    return best;
}
