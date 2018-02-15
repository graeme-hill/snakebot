#include "sim.hpp"
#include "dog.hpp"
#include "hungry.hpp"
#include "cautious.hpp"
#include "../movement.hpp"
#include "../astar.hpp"
#include "../simulator.hpp"

#include <functional>
#include <unordered_map>

Metadata Sim::meta()
{
    return {
        "#008800",
        "#FFFFFF",
        "https://news.filehippo.com/wp-content/uploads/2014/12/www.moma_.org_.jpg",
        "C++ sim",
        "Hi",
        "pixel",
        "pixel"
    };
}

Direction Sim::move(GameState &state)
{
    Dog dog;
    Hungry hungry;
    Cautious cautious;

    std::vector<Algorithm *> algorithms { &cautious };
    std::vector<Future> futures = simulateFutures(
    	state, 100, algorithms);
    Direction best = bestMove(futures, state);

    return best;
}