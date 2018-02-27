#pragma once

#include "../snakelib.hpp"

class InYourFace : public Algorithm
{
public:
    InYourFace();
    InYourFace(Snake *target);
    Metadata meta() override;
    Direction move(GameState &state) override;
    void start(std::string id) override;

private:
    Snake *_target;
};
