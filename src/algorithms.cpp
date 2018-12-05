#include "algorithms.hpp"
#include "algorithms/cautious.hpp"
#include "algorithms/hungry.hpp"
#include "algorithms/terminator.hpp"
#include "algorithms/dog.hpp"
#include "algorithms/sim.hpp"
#include "algorithms/inyourface.hpp"
#include "algorithms/random.hpp"
#include "algorithms/onedirection.hpp"

Algorithms Algorithms::_instance;

Algorithms::Algorithms()
{
    _algorithms["cautious"] = std::make_unique<Cautious>();
    _algorithms["hungry"] = std::make_unique<Hungry>();
    _algorithms["terminator"] = std::make_unique<Terminator>();
    _algorithms["dog"] = std::make_unique<Dog>();
    _algorithms["sim"] = std::make_unique<Sim>();
    _algorithms["inyourface"] = std::make_unique<InYourFace>();
    _algorithms["random"] = std::make_unique<Random>();
    _algorithms["onedirection_left"] = std::make_unique<OneDirection>(Direction::Left);
    _algorithms["onedirection_right"] = std::make_unique<OneDirection>(Direction::Right);
    _algorithms["onedirection_up"] = std::make_unique<OneDirection>(Direction::Up);
    _algorithms["onedirection_down"] = std::make_unique<OneDirection>(Direction::Down);
}

Algorithm *Algorithms::get(std::string key)
{
    auto it = _instance._algorithms.find(key);
    if (it == _instance._algorithms.end())
    {
        return nullptr;
    }
    return it->second.get();
}
