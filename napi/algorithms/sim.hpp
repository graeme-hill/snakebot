#pragma once

#include "../snakelib.hpp"

class Sim : public Algorithm
{
public:
    Metadata meta() override;
    Direction move(GameState &state) override;
    void start() override;
};
