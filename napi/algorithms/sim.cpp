#include "sim.hpp"
#include "hungry.hpp"
#include "cautious.hpp"
#include "inyourface.hpp"
#include "terminator.hpp"
#include "random.hpp"
#include "dog.hpp"
#include "onedirection.hpp"
#include "../movement.hpp"
#include "../astar.hpp"
#include "../simulator.hpp"

#include <functional>
#include <unordered_map>

Sim::Sim() : _maxTurns(10000), _maxMillis(120)
{ }

Sim::Sim(uint32_t maxTurns, uint32_t maxMillis) :
    _maxTurns(maxTurns), _maxMillis(maxMillis)
{ }

Metadata Sim::meta()
{
    return {
        "#698866",
        "#698866",
        "http://www.theglamnationnetwork.com/uploads/1/3/1/2/13123626/6958010.jpg",
        "White Snake",
        "Hi",
        "shades",
        "pixel"
    };
}

void Sim::start(std::string /*id*/)
{
    // Make sure the simulation threads are all awake so that it's not slow on
    // the first move.
    SimThread::wakeAll();
}

Direction Sim::move(GameState &state)
{
    Direction l = Direction::Left;
    Direction r = Direction::Right;
    Direction u = Direction::Up;
    Direction d = Direction::Down;

    std::vector<std::vector<Direction>> myPrefixMoves {
        { u, u },
        { u, l },
        { u, r },
        { d, d },
        { d, l },
        { d, r },
        { l, l },
        { l, u },
        { l, d },
        { r, r },
        { r, u },
        { r, d }
    };

    std::vector<std::vector<Direction>> enemyPrefixMoves {
        {l},{r},{u},{d}
    };
    InYourFace inYourFace;
    InYourFace inMyFace(state.mySnake());
    Hungry hungry;
    Cautious cautious;
    Terminator terminator;
    Random random;
    Dog dog;

    OneDirection left(Direction::Left);
    OneDirection right(Direction::Right);
    OneDirection up(Direction::Up);
    OneDirection down(Direction::Down);

    std::vector<PrefixedAlgorithm> myAlgorithms {
        { &dog, myPrefixMoves },
        { &cautious, {} },
        { &inYourFace, {} },
    };
    std::vector<PrefixedAlgorithm> enemyAlgorithms {
        { &hungry, { } },
        { &inMyFace, enemyPrefixMoves },
        { &left, { } },
        { &right, { } },
        { &up, { } },
        { &down, { } },
    };

    Direction preferred = dog.move(state);

    std::vector<Future> futures = simulateFutures(
    	state, _maxTurns, _maxMillis, myAlgorithms, enemyAlgorithms);

    Direction best = bestMove(futures, state, MaybeDirection::just(preferred));

    return best;
}
