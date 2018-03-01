#include "cautious.hpp"
#include "../movement.hpp"
#include "../astar.hpp"

#include <functional>
#include <unordered_map>

Metadata Cautious::meta()
{
    return {
        "#880000",
        "#FFFFFF",
        "https://www.illustrationsof.com/royalty-free-cautious-clipart-illustration-1046454.jpg",
        "Cautious",
        "Hi",
        "pixel",
        "pixel"
    };
}

void Cautious::start(std::string /*id*/)
{
}

Direction Cautious::move(GameState &state)
{
    auto foodDir = bestFood(state);
    if (foodDir.hasValue)
    {
        return foodDir.value;
    }

    auto chaseDir = chaseTail(state);
    if (chaseDir.hasValue)
    {
        return chaseDir.value;
    }

    auto alt = notImmediatelySuicidal(state);
    return alt.value;
}
