#pragma once

#include <string>
#include "snakelib.hpp"

class Dispatcher
{
public:
    static std::string move(std::string json);
    static std::string start(std::string json);

    static Algorithm *algorithm;
};
