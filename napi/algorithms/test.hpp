#pragma once

#include "../snakelib.hpp"

class Test : public Algorithm
{
public:
	Metadata meta() override;
	Direction move(World world) override;
};