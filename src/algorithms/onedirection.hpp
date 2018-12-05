#pragma once

#include "../snakelib.hpp"

class OneDirection : public Algorithm
{
public:
    OneDirection(Direction direction);
    Metadata meta() override;
    Direction move(GameState &state) override;
    Direction move(GameState &state, uint32_t branchId) override;
    void start(std::string id) override;

private:
    Direction _direction;

    static std::unordered_map<std::string, bool> _finishMap;
};
