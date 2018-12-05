#pragma once

#include "../snakelib.hpp"

class Random : public Algorithm
{
public:
    Metadata meta() override;
    Direction move(GameState &state) override;
    void start(std::string id) override;
};
