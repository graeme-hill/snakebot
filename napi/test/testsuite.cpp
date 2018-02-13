#include "testsuite.hpp"
#include "../interop.hpp"
#include <iostream>

void basicGameStateTests()
{
    GameState state(parseWorld({
        "> > 0 _",
        "_ * _ *",
        "_ 1 _ _",
        "_ ^ < <",
        "_ _ _ _"
    }));

    auto snakes = state.snakes();
    auto snake0 = snakes["0"];
    auto snake1 = snakes["1"];

    std::cout << "#################\n";
    snake0->prettyPrint();
    std::cout << "#################\n";

    assertEqual(state.width(), 4, "basicGameStateTests() - width");
    assertEqual(state.height(), 5, "basicGameStateTests() - height");
    assertEqual(snakes.size(), 2, "basicGameStateTests() - snake count");
    assertTrue(state.mySnake() == snake0, "basicGameStateTests() - mySnake is snake0");
    assertEqual(state.mySnake()->id, "0", "basicGameStateTests() - my ID");
    assertEqual(snake0->id, "0", "basicGameStateTests() - snake0 ID");
    // assertEqual(snakes["0"]->id, "0", "basicGameStateTests() - first snake ID");
    // assertEqual(snakes["0"]->health, 100, "basicGameStateTests() - first snake health");
    // assertEqual(snakes["0"]->parts.size(), 3, "basicGameStateTests() - first snake parts count");
    // assertEqual(snakes["1"]->id, "1", "basicGameStateTests() - second snake ID");
    // assertEqual(snakes["1"]->health, 100, "basicGameStateTests() - second snake health");
    // assertEqual(snakes["1"]->parts.size(), 4, "basicGameStateTests() - second snake parts count");
}

void TestSuite::run()
{
    basicGameStateTests();
}