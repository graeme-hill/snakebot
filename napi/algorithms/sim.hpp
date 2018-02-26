#pragma once

#include "../snakelib.hpp"

class Sim : public Algorithm
{
public:
    Sim();
    Sim(uint32_t maxTurns, uint32_t maxMillis);
    Metadata meta() override;
    Direction move(GameState &state) override;
    void start(std::string id) override;

private:
    uint32_t _maxTurns;
    uint32_t _maxMillis;
};
