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
    auto food = state.food();

    assertEqual(state.width(), 4, "basicGameStateTests() - width");
    assertEqual(state.height(), 5, "basicGameStateTests() - height");
    assertEqual(snakes.size(), 2, "basicGameStateTests() - snake count");
    assertTrue(state.mySnake() == snake0, "basicGameStateTests() - mySnake is snake0");
    assertEqual(state.mySnake()->id, "0", "basicGameStateTests() - my ID");
    assertEqual(snake0->id, "0", "basicGameStateTests() - first snake ID");
    assertEqual(snake0->health, 100, "basicGameStateTests() - first snake health");
    assertEqual(snake0->parts.size(), 3, "basicGameStateTests() - first snake parts count");
    assertEqual(snake1->id, "1", "basicGameStateTests() - second snake ID");
    assertEqual(snake1->health, 100, "basicGameStateTests() - second snake health");
    assertEqual(snake1->parts.size(), 4, "basicGameStateTests() - second snake parts count");

    assertTrue(snake0->parts[0] == snake0->head(), "basicGameStateTests() - head() is first");
    assertTrue(snake0->parts[2] == snake0->tail(), "basicGameStateTests() - tail() is last");

    snake0->prettyPrint();
    snake1->prettyPrint();

    assertTrue(snake0->parts[0] == Point{2,0}, "basicGameStateTests() - snake0 part0");
    assertTrue(snake0->parts[1] == Point{1,0}, "basicGameStateTests() - snake0 part1");
    assertTrue(snake0->parts[2] == Point{0,0}, "basicGameStateTests() - snake0 part2");

    assertTrue(snake1->parts[0] == Point{1,2}, "basicGameStateTests() - snake1 part0");
    assertTrue(snake1->parts[1] == Point{1,3}, "basicGameStateTests() - snake1 part1");
    assertTrue(snake1->parts[2] == Point{2,3}, "basicGameStateTests() - snake1 part2");
    assertTrue(snake1->parts[3] == Point{3,3}, "basicGameStateTests() - snake1 part3");

    assertEqual(food.size(), 2, "basicGameStateTests() - 2 foods");

    // Order doesn't matter so make it work either way.
    if (food[0] == Point{1,1})
    {
        assertTrue(food[0] == Point{1,1}, "basicGameStateTests() - first food position");
        assertTrue(food[1] == Point{3,1}, "basicGameStateTests() - second food position");
    }
    else
    {
        assertTrue(food[0] == Point{3,1}, "basicGameStateTests() - first food position");
        assertTrue(food[1] == Point{1,1}, "basicGameStateTests() - second food position");
    }
}

void TestSuite::run()
{
    basicGameStateTests();
}