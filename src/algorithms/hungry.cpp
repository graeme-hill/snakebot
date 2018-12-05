#include "hungry.hpp"
#include "../movement.hpp"
#include "../astar.hpp"

#include <functional>
#include <unordered_map>

Metadata Hungry::meta()
{
    return {
        "#008888",
        "#FFFFFF",
        "http://residentevents.com/media/reviews/photos/original/84/ad/76/hungry20hungry20hippos-78-1440787989.jpg",
        "Hungry",
        "Yum",
        "pixel",
        "pixel"
    };
}

void Hungry::start(std::string /*id*/)
{
}

Direction Hungry::move(GameState &state)
{
    auto foodDir = closestFood(state);
    if (foodDir.hasValue)
    {
        return foodDir.value;
    }

    auto alt = notImmediatelySuicidal(state);
    return alt.value;
}
