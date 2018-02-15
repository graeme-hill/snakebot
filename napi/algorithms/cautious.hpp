#pragma once

#include "../snakelib.hpp"

class Cautious : public Algorithm
{
public:
    Metadata meta() override;
    Direction move(GameState &state) override;
};