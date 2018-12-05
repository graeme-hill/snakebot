#include "random.hpp"
#include "../movement.hpp"
#include "../astar.hpp"

#include <functional>
#include <unordered_map>

Metadata Random::meta()
{
    return {
        "#000088",
        "#FFFFFF",
        "http://residentevents.com/media/reviews/photos/original/84/ad/76/hungry20hungry20hippos-78-1440787989.jpg",
        "Dog",
        "Woof",
        "pixel",
        "pixel"
    };
}

void Random::start(std::string /*id*/)
{
}

Direction Random::move(GameState &state)
{
    auto move = notImmediatelySuicidal(state);
    return move.value;
}
