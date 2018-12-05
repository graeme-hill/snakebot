#pragma once

#include "../snakelib.hpp"

class Hungry : public Algorithm
{
public:
    Metadata meta() override;
    Direction move(GameState &state) override;
    void start(std::string id) override;
};
