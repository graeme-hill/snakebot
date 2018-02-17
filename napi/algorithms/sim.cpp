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
        "https://www.popoptiq.com/wp-content/uploads/2015/04/Timecop-1994-Movie-Picture-01.jpg",
        "Time Cop",
        "Hi",
        "shades",
        "fat-rattle"
    };
}

Direction Sim::move(GameState &state)
{
    Dog dog;
    Hungry hungry;
    Cautious cautious;

    std::vector<Algorithm *> myAlgorithms { &cautious };
    std::vector<Algorithm *> enemyAlgorithms { &hungry };
    std::vector<Future> futures = simulateFutures(
    	state, 100, myAlgorithms, enemyAlgorithms);
    Direction best = bestMove(futures, state);

    return best;
}