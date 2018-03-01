#include "dog.hpp"
#include "../movement.hpp"
#include "../astar.hpp"

#include <functional>
#include <unordered_map>

Metadata Dog::meta()
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

void Dog::start(std::string /*id*/)
{
}

Direction Dog::move(GameState &state)
{

    auto chaseDir = chaseTail(state);
    if (chaseDir.hasValue)
    {
        return chaseDir.value;
    }

    auto alt = notImmediatelySuicidal(state);
    return alt.value;
}
