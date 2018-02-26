#pragma once

#include "../snakelib.hpp"

class OneDirection : public Algorithm
{
public:
    OneDirection(Direction direction);
    Metadata meta() override;
    Direction move(GameState &state) override;
    void start(std::string id) override;

private:
    Direction _direction;
};
