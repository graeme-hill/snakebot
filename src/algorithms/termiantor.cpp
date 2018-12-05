#include "terminator.hpp"
#include "../movement.hpp"
#include "../astar.hpp"

#include <functional>
#include <unordered_map>

Metadata Terminator::meta()
{
    return {
        "#ff6347",
        "#d8bfd8",
        "http://webiconspng.com/wp-content/uploads/2017/09/Terminator-PNG-Image-10416.png",
        "Terminator",
        "Two Weeks, Two Weeks, two weeeeks",
        "pixel",
        "pixel"
    };
}

void Terminator::start(std::string /*id*/)
{
}

Direction Terminator::move(GameState &state)
{
    auto killTunnelDir = closestKillTunnelTarget(state, 2);
    if (killTunnelDir.hasValue)
    {
        return killTunnelDir.value;
    }

    auto foodDir = closestFood(state);
    if (foodDir.hasValue)
    {
        return foodDir.value;
    }

    auto alt = notImmediatelySuicidal(state);
    return alt.value;
}
