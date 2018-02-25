#include "testsuite.hpp"
#include "../interop.hpp"
#include "../astar.hpp"
#include "../movement.hpp"
#include "../simulator.hpp"
#include "../algorithms/sim.hpp"
#include "../algorithms/inyourface.hpp"
#include "../algorithms/cautious.hpp"
#include <iostream>

class OneDirAlgorithm : public Algorithm
{
public:
    OneDirAlgorithm(Direction dir) : _dir(dir) { }

    Metadata meta() override
    {
        return {
            "#000000",
            "#FFFFFF",
            "http://www.edm2.com/images/thumb/1/13/C%2B%2B.png/250px-C%2B%2B.png",
            "C++ test",
            "Hi",
            "pixel",
            "pixel"
        };
    }

    void start() override
    { }

    Direction move(GameState &) override
    {
        return _dir;
    }

private:
    Direction _dir;
};

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

void mapTests()
{
    GameState state(parseWorld({
        "> > 0 _",
        "_ * _ *",
        "_ 1 _ _",
        "_ ^ < <",
        "_ _ _ _"
    }));

    Map &map = state.map();

    assertEqual(map.turnsUntilVacant({0,0}), 0, "mapTests() - snake0 part vacant after 0 turns");
    assertEqual(map.turnsUntilVacant({1,0}), 1, "mapTests() - snake0 part vacant after 1 turn");
    assertEqual(map.turnsUntilVacant({2,0}), 2, "mapTests() - snake0 part vacant after 2 turns");

    assertEqual(map.turnsUntilVacant({3,3}), 0, "mapTests() - snake1 part vacant after 0 turns");
    assertEqual(map.turnsUntilVacant({2,3}), 1, "mapTests() - snake1 part vacant after 1 turn");
    assertEqual(map.turnsUntilVacant({1,3}), 2, "mapTests() - snake1 part vacant after 2 turns");
    assertEqual(map.turnsUntilVacant({1,2}), 3, "mapTests() - snake1 part vacant after 3 turns");

    assertEqual(map.turnsUntilVacant({3,0}), 0, "mapTests() - empty cell vacant");
    assertEqual(map.turnsUntilVacant({0,1}), 0, "mapTests() - empty cell vacant");
    assertEqual(map.turnsUntilVacant({1,1}), 0, "mapTests() - empty cell vacant");
    assertEqual(map.turnsUntilVacant({2,1}), 0, "mapTests() - empty cell vacant");
    assertEqual(map.turnsUntilVacant({3,1}), 0, "mapTests() - empty cell vacant");
    assertEqual(map.turnsUntilVacant({0,2}), 0, "mapTests() - empty cell vacant");
    assertEqual(map.turnsUntilVacant({2,2}), 0, "mapTests() - empty cell vacant");
    assertEqual(map.turnsUntilVacant({3,2}), 0, "mapTests() - empty cell vacant");
    assertEqual(map.turnsUntilVacant({0,3}), 0, "mapTests() - empty cell vacant");
    assertEqual(map.turnsUntilVacant({0,4}), 0, "mapTests() - empty cell vacant");
    assertEqual(map.turnsUntilVacant({1,4}), 0, "mapTests() - empty cell vacant");
    assertEqual(map.turnsUntilVacant({2,4}), 0, "mapTests() - empty cell vacant");
    assertEqual(map.turnsUntilVacant({3,4}), 0, "mapTests() - empty cell vacant");
}

void outOfBoundsTests()
{
    GameState state(parseWorld({
        "> > 0 _",
        "_ * _ *",
        "_ 1 _ _",
        "_ ^ < <",
        "_ _ _ _"
    }));

    assertTrue(outOfBounds({4, 0}, state), "outOfBoundsTests() - off right edge");
    assertTrue(outOfBounds({0, 5}, state), "outOfBoundsTests() - off bottom edge");
    assertTrue(!outOfBounds({0, 0}, state), "outOfBoundsTests() - in bounds");
}

void astarTests1()
{
    GameState state(parseWorld({
        "> > 0 _",
        "_ _ _ _",
        "_ _ _ _",
        "_ _ _ _"
    }));

    auto path = shortestPath({2,0}, {3,1}, state);
    assertEqual(path.size, 2, "astarTests1() - simple case path length");

    // Doesn't matter whether it goes down,right or right,down.
    if (path.direction.value == Direction::Down)
    {
        assertEqual(path.direction.value, Direction::Down, "astarTests1() - down first");
    }
    else
    {
        assertEqual(path.direction.value, Direction::Right, "astarTests1() - right first");
    }
}

void astarTests2()
{
    GameState state(parseWorld({
        "> > 0 _",
        "_ v 1 _",
        "_ > ^ _",
        "_ _ _ _"
    }));

    // Expected path will be right,down,down,left,left,left
    auto path = shortestPath({2,0}, {0,2}, state);
    assertEqual(path.size, 6, "astarTests2() - path length");

    assertEqual(path.direction.value, Direction::Right, "astarTests2() - right");
}

void astarTests3()
{
    GameState state(parseWorld({
        "_ _ _ _ _ _",
        "_ _ 1 _ _ _",
        "v _ ^ 2 _ _",
        "> 0 ^ ^ _ _",
        "_ _ _ ^ _ _",
        "_ _ _ _ _ _",
    }));

    // Expected path will be right,right,right
    Path path = shortestPath({1,3}, {4,3}, state);
    assertEqual(path.size, 3, "astarTests3() - path length");

    assertEqual(path.direction.value, Direction::Right, "astarTests3() - right");
}

void astarTests4()
{
    GameState state(parseWorld({
        "_ _ _ _ _ _",
        "v _ 1 _ _ _",
        "> 0 ^ 2 _ _",
        "_ _ ^ ^ _ _",
        "_ _ ^ ^ _ _",
        "_ _ _ ^ _ _",
    }));

    // Expected path will be down,right,right,right
    Path path = shortestPath({1,2}, {4,3}, state);
    assertEqual(path.size, 4, "astarTests4() - path length");

    assertEqual(path.direction.value, Direction::Down, "astarTests4() - down");
}

void astarTests5()
{
    GameState state(parseWorld({
        "_ _ v < _ _",
        "v _ 1 _ _ _",
        "> 0 _ * _ _",
        "_ _ _ _ _ _",
        "_ _ _ _ _ _",
        "_ _ _ _ _ _",
    }));

    Path path = shortestPath({1,2}, {3,2}, state);
    assertEqual(path.size, 4, "astarTests5() - path length");

    assertEqual(path.direction.value, Direction::Down, "astarTests5() - down to avoid getting eaten");
}

void astarTests6()
{
    // Created this test based on a real scenario that caused an infinite loop
    // because both initial choices are dangerous.

    GameState state(parseWorld({
        "_ v < < < _ _ _",
        "_ > > v ^ _ _ _",
        "_ v _ 1 _ _ _ _",
        "_ v 0 _ _ _ _ *",
        "_ > ^ _ _ _ _ _",
        "_ _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _"
    }));

    Path path = shortestPath({2,3}, {7,3}, state);
    assertEqual(path.size, 5, "astarTests6() - path length");
}

void closestFoodTest1()
{
    GameState state(parseWorld({
        "> > 0 _",
        "_ _ _ _",
        "1 < < _",
        "_ _ * _"
    }));

    auto direction = closestFood(state);
    assertTrue(direction.hasValue, "closestFoodTest1() - can find food");
    assertTrue(direction.value == Direction::Down, "closestFoodTest1() - down");
}

void closestFoodTest2()
{
    GameState state(parseWorld({
        "> > 0 _",
        "_ > > v",
        "_ _ v <",
        "_ _ 1 *"
    }));

    auto direction = closestFood(state);
    assertTrue(!direction.hasValue, "closestFoodTest2() - inaccessible food");
}

void closestFoodTest3()
{
    GameState state(parseWorld({
        "> > 0 _",
        "_ _ _ _",
        "_ _ v <",
        "_ _ 1 _"
    }));

    auto direction = closestFood(state);
    assertTrue(!direction.hasValue, "closestFoodTest3() - no food");
}

void bestFoodTest1()
{
    GameState state(parseWorld({
        "> > 0 _",
        "_ _ _ _",
        "_ * v <",
        "* _ 1 _"
    }));

    auto foodDir = bestFood(state);
    assertTrue(!foodDir.hasValue, "bestFoodTest1() - no easy food");
}

void bestFoodTest2()
{
    GameState state(parseWorld({
        "> > 0 _",
        "_ * _ _",
        "_ * v <",
        "* _ 1 _"
    }));

    auto foodDir = bestFood(state);
    assertTrue(foodDir.hasValue, "bestFoodTest2() - one easy food");
    assertTrue(foodDir.value == Direction::Down, "bestFoodTest2() - down");
}

void bestFoodTest3()
{
    GameState state(parseWorld({
        "> > 0 _",
        "_ _ _ _",
        "_ _ v <",
        "_ _ 1 _"
    }));

    auto foodDir = bestFood(state);
    assertTrue(!foodDir.hasValue, "bestFoodTest3() - no food at all");
}

void notImmediatelySuicidalTest1()
{
    GameState state(parseWorld({
        "> > > 1",
        "> 0 v <",
        "_ _ v _",
        "_ _ 2 _"
    }));

    auto direction = notImmediatelySuicidal(state);
    assertTrue(direction.hasValue, "notImmediatelySuicidalTest1() - hasValue");
    assertTrue(direction.value == Direction::Down, "notImmediatelySuicidalTest1() - down");
}

void notImmediatelySuicidalTest2()
{
    GameState state(parseWorld({
        "> > > 1",
        "> 0 v <",
        "> v v _",
        "_ 3 2 _"
    }));

    auto direction = notImmediatelySuicidal(state);
    assertTrue(!direction.hasValue, "notImmediatelySuicidalTest2() - !hasValue");
}

void worldComparisonTest1()
{
    // This is basically a meta test to make sure test helper function for
    // comparing world objects is at least somewhat reliable.

    World w1 = parseWorld({
        "> > > 1",
        "> 0 v <",
        "> v v _",
        "_ 3 2 _"
    });

    World w2 = parseWorld({
        "> > > 1",
        "> 0 v <",
        "> v v _",
        "_ 3 2 _"
    });

    assertEqual(w1, w2, "world compare");
}

void newStateAfterMovesTest1()
{
    GameState state(parseWorld({
        "> > 0 _",
        "_ _ _ _",
        "_ 1 _ _",
        "_ ^ < *"
    }));

    Snake *s0 = state.snakes()["0"];
    Snake *s1 = state.snakes()["1"];

    std::vector<SnakeMove> moves {
        { s0, Direction::Down },
        { s1, Direction::Left }
    };
    auto newState = state.newStateAfterMoves(moves);

    World expected = parseWorld({
        "_ > v _",
        "_ _ 0 _",
        "1 < _ _",
        "_ ^ _ *"
    });

    assertEqual(newState->world(), expected, "newStateAfterMovesTest1()");
}

void newStateAfterMovesTest2()
{
    GameState state(parseWorld({
        "> > 0 *",
        "_ _ _ _",
        "_ _ _ _",
        "_ _ _ _"
    }));

    Snake *s0 = state.snakes()["0"];

    std::vector<SnakeMove> moves {
        { s0, Direction::Right },
    };
    auto newState = state.newStateAfterMoves(moves);

    World expected = parseWorld({
        "_ > > 0",
        "_ _ _ _",
        "_ _ _ _",
        "_ _ _ _"
    });

    // Make sure the snake has double tail end piece
    expected.snakes[0].parts.push_back(expected.snakes[0].parts.at(2));

    assertEqual(newState->world(), expected, "newStateAfterMovesTest2()");
}

void newStateAfterMovesTest3()
{
    GameState state(parseWorld({
        "> > 0 v",
        "_ _ _ v",
        "_ _ _ 1",
        "_ _ _ _"
    }));

    Snake *s0 = state.snakes()["0"];
    Snake *s1 = state.snakes()["1"];

    std::vector<SnakeMove> moves {
        { s0, Direction::Right },
        { s1, Direction::Left },
    };
    auto newState = state.newStateAfterMoves(moves);

    World expected = parseWorld({
        "_ > > 0",
        "_ _ _ v",
        "_ _ 1 <",
        "_ _ _ _"
    });

    assertEqual(newState->world(), expected, "newStateAfterMovesTest3()");
}

void newStateAfterMovesTest4()
{
    GameState state(parseWorld({
        "_ > > 0",
        "_ _ 1 _",
        "_ _ ^ <",
        "_ _ _ _"
    }));

    Snake *s0 = state.snakes()["0"];
    Snake *s1 = state.snakes()["1"];

    std::vector<SnakeMove> moves {
        { s0, Direction::Down },
        { s1, Direction::Right },
    };
    auto newState = state.newStateAfterMoves(moves);

    World expected = parseWorld({
        "_ _ _ _",
        "_ _ _ _",
        "_ _ _ _",
        "_ _ _ _"
    });

    assertEqual(newState->world(), expected, "newStateAfterMovesTest4()");
}

void newStateAfterMovesTest5()
{
    GameState state(parseWorld({
        "> > > 0",
        "_ _ 1 _",
        "_ _ ^ <",
        "_ _ _ _"
    }));

    Snake *s0 = state.snakes()["0"];
    Snake *s1 = state.snakes()["1"];

    std::vector<SnakeMove> moves {
        { s0, Direction::Down },
        { s1, Direction::Right },
    };
    auto newState = state.newStateAfterMoves(moves);

    World expected = parseWorld({
        "_ > > v",
        "_ _ _ 0",
        "_ _ _ _",
        "_ _ _ _"
    });

    assertEqual(newState->world(), expected, "newStateAfterMovesTest5()");
}

void newStateAfterMovesTest6()
{
    GameState state(parseWorld({
        "_ v < _",
        "_ 0 _ _",
        "1 _ 2 <",
        "^ 3 < _"
    }));

    Snake *s0 = state.snakes()["0"];
    Snake *s1 = state.snakes()["1"];
    Snake *s2 = state.snakes()["2"];
    Snake *s3 = state.snakes()["3"];

    std::vector<SnakeMove> moves {
        { s0, Direction::Down },
        { s1, Direction::Right },
        { s2, Direction::Left },
        { s3, Direction::Up }
    };
    auto newState = state.newStateAfterMoves(moves);

    World expected = parseWorld({
        "_ v _ _",
        "_ v _ _",
        "_ 0 _ _",
        "_ _ _ _"
    });

    assertEqual(newState->world(), expected, "newStateAfterMovesTest6()");
}

void newStateAfterMovesTest7()
{
    GameState state(parseWorld({
        "> > > 0",
        "_ _ 1 _",
        "_ _ ^ <",
        "_ _ _ _"
    }));

    Snake *s0 = state.snakes()["0"];
    Snake *s1 = state.snakes()["1"];

    std::vector<SnakeMove> moves {
        { s0, Direction::Down },
        { s1, Direction::Up },
    };
    auto newState = state.newStateAfterMoves(moves);

    World expected = parseWorld({
        "_ > > v",
        "_ _ _ 0",
        "_ _ _ _",
        "_ _ _ _"
    });

    assertEqual(newState->world(), expected, "newStateAfterMovesTest7()");
}

void simulateFuturesTest1()
{
    GameState state(parseWorld({
        "* _ _ _",
        "* _ _ _",
        "_ 0 1 _",
        "_ ^ ^ _"
    }));

    OneDirAlgorithm algo(Direction::Up);

    std::vector<Future> actualFutures = simulateFutures(
        state, 5, 100, { &algo }, { &algo });

    Future f1 {};
    f1.obituaries = { { "1", 3 }, { "0", 4 } };
    f1.foodsEaten = { { "0", { 2, 3 } } };
    f1.algorithm = &algo;
    f1.terminationReason = TerminationReason::Loss;
    f1.move = Direction::Left;
    f1.turns = 4;

    Future f2 {};
    f2.obituaries = { { "1", 3 }, { "0", 3 } };
    f2.foodsEaten = {};
    f2.algorithm = &algo;
    f2.terminationReason = TerminationReason::Loss;
    f2.move = Direction::Up;
    f2.turns = 3;

    Future f3 {};
    f3.obituaries = { { "1", 3 }, { "0", 4 } };
    f3.foodsEaten = { { "0", { 2, 3 } } };
    f3.algorithm = &algo;
    f3.terminationReason = TerminationReason::Loss;
    f3.move = Direction::Left;
    f3.turns = 4;

    Future f4 {};
    f4.obituaries = { { "1", 3 }, { "0", 3 } };
    f4.foodsEaten = {};
    f4.algorithm = &algo;
    f4.terminationReason = TerminationReason::Loss;
    f4.move = Direction::Up;
    f4.turns = 3;

    std::vector<Future> expectedFutures = { f1, f3, f2, f4 };

    assertEqual(actualFutures, expectedFutures, "simulateFuturesTest1()");
}

void simulateFuturesTest2()
{
    GameState state(parseWorld({
        "_ _ _ _ _ _ _ _ _ > > > v _ _ _ _",
        "_ _ _ _ _ _ _ v _ ^ _ _ > > v _ _",
        "* _ _ _ _ _ _ v * ^ _ _ _ _ > > 0",
        "* _ * _ * _ 2 > > > > > > > > v _",
        "v < _ _ _ _ ^ _ _ _ _ _ _ _ _ v _",
        "v ^ _ _ _ _ ^ _ _ _ * _ _ _ _ v _",
        "v _ _ _ _ _ ^ _ _ _ _ _ _ _ _ > v",
        "v _ _ _ _ * ^ _ _ 1 < _ _ _ _ _ v",
        "v _ _ * * _ ^ _ * _ ^ < < _ _ _ v",
        "v _ _ _ _ _ ^ _ _ _ _ _ ^ _ v < <",
        "v _ _ _ * _ ^ _ _ _ * _ ^ < < _ _",
        "> > > > > > ^ _ _ _ _ _ _ _ _ _ _"
    }));

    Cautious cautious;
    AlgorithmPair pair { &cautious, &cautious };
    AxisBias bias = AxisBias::Horizontal;
    AlgorithmBranch branch { pair, MaybeDirection::just(Direction::Up), bias };
    std::vector<AlgorithmBranch> branches { branch };
    auto futures = runSimulationBranches(branches, state, 20, 1000);

    assertEqual(futures.size(), 1, "simulateFuturesTest2() - one future");
    Future future = futures.at(0);
    TerminationReason reason = future.terminationReason;
    assertEqual(reason, TerminationReason::MaxTurns, "simulateFuturesTest2() - should lose");
}

void bestMoveTest1()
{
    GameState state(parseWorld({
        "_ _ _ _",
        "_ _ _ _",
        "_ 0 1 _",
        "_ ^ ^ _"
    }));

    OneDirAlgorithm algo(Direction::Up);

    Future f1 {};
    f1.obituaries = { { "1", 3 }, { "0", 4 } };
    f1.foodsEaten = {};
    f1.algorithm = &algo;
    f1.terminationReason = TerminationReason::Loss;
    f1.move = Direction::Left;
    f1.turns = 4;

    Future f2 {};
    f2.obituaries = { { "1", 3 }, { "0", 3 } };
    f2.foodsEaten = {};
    f2.algorithm = &algo;
    f2.terminationReason = TerminationReason::Loss;
    f2.move = Direction::Up;
    f2.turns = 3;

    std::vector<Future> futures = { f1, f2 };

    Direction move = bestMove(futures, state);

    assertEqual(move, Direction::Left, "bestMoveTest1() - left first");
}

void directionSetTests()
{
    {
        DirectionSet set;
        set.push(Direction::Right);

        std::vector<Direction> vec;
        for (Direction d : set)
        {
            vec.push_back(d);
        }

        assertEqual(vec.size(), 1, "DirectionSetTests() - size");
        assertEqual(vec.at(0), Direction::Right, "DirectionSetTests() - direction");
    }

    {
        DirectionSet set;
        set.push(Direction::Up);
        set.push(Direction::Down);
        set.push(Direction::Up);

        std::vector<Direction> vec;
        for (Direction d : set)
        {
            vec.push_back(d);
        }

        assertEqual(vec.size(), 2, "DirectionSetTests() - size");
        assertEqual(vec.at(0), Direction::Up, "DirectionSetTests() - direction");
        assertEqual(vec.at(1), Direction::Down, "DirectionSetTests() - direction");
    }

    {
        DirectionSet set;
        set.push(Direction::Up);
        set.push(Direction::Right);
        set.push(Direction::Down);
        set.push(Direction::Left);

        std::vector<Direction> vec;
        for (Direction d : set)
        {
            vec.push_back(d);
        }

        assertEqual(vec.size(), 4, "DirectionSetTests() - size");
        assertEqual(vec.at(0), Direction::Left, "DirectionSetTests() - direction");
        assertEqual(vec.at(1), Direction::Right, "DirectionSetTests() - direction");
        assertEqual(vec.at(2), Direction::Up, "DirectionSetTests() - direction");
        assertEqual(vec.at(3), Direction::Down, "DirectionSetTests() - direction");
    }
}

void arrayDictTest1()
{
    ArrayDict<std::string, 10> test;
    test.set(3, "three");
    test.set(7, "seven");
    assertEqual(test.get(3).value(), "three", "snakeDictTest1 - 3");
    assertEqual(test.get(7).value(), "seven", "snakeDictTest1 - 7");
    assertTrue(!test.get(0).hasValue(), "snakeDictTest1 - 0 !exists");
    assertTrue(!test.get(10).hasValue(), "snakeDictTest1 - 10 !exists");
    assertTrue(test.get(3).hasValue(), "snakeDictTest1 - 3 exists");
    assertTrue(test.get(7).hasValue(), "snakeDictTest1 - 7 exists");

    test.remove(3);
    test.remove(9);

    assertTrue(!test.get(10).hasValue(), "snakeDictTest1 - 10 !exists");
    assertTrue(!test.get(3).hasValue(), "snakeDictTest1 - 3 !exists");
    assertTrue(test.get(7).hasValue(), "snakeDictTest1 - 7 exists");
}

void wideRectangleTest1()
{
    GameState state(parseWorld({
        "_ _ _ _ _ _ _ _ _ v _ _",
        "_ _ * _ _ _ 1 < < < _ _",
        "_ _ > v _ _ _ * _ _ _ _",
        "_ _ _ > 0 _ * _ _ _ _ _"
    }));

    assertEqual(state.width(), 12, "wideRectangleTest1() - width");
    assertEqual(state.height(), 4, "wideRectangleTest1() - height");

    Sim sim;
    sim.move(state);
    assertTrue(true, "wideRectangleTest1() - don't segfault");
}

void tallRectangleTest1()
{
    GameState state(parseWorld({
        "_ _ _ _",
        "_ _ _ _",
        "_ < < <",
        "_ v _ _",
        "1 < _ _",
        "_ * _ *",
        "_ _ _ _",
        "_ _ _ _",
        "* _ 0 <",
        "_ _ > ^",
        "_ _ ^ _",
        "_ _ _ _"
    }));

    assertEqual(state.width(), 4, "wideRectangleTest1() - width");
    assertEqual(state.height(), 12, "wideRectangleTest1() - height");

    Sim sim;
    sim.move(state);
    assertTrue(true, "tallRectangleTest1() - don't segfault");
}

void inYourFaceTest1()
{
    GameState state(parseWorld({
        "_ _ _ _ _",
        "_ _ _ 0 <",
        "_ 1 < < _",
        "_ _ _ _ _"
    }));

    InYourFace inYourFace;
    Direction dir = inYourFace.move(state);
    assertEqual(dir, Direction::Left, "inYourFaceTest1() - go left");
}

void inYourFaceTest2()
{
    GameState state(parseWorld({
        "_ 0 < <",
        "_ _ _ _",
        "_ _ _ _",
        "_ 1 < <",
        "_ _ _ _"
    }));

    InYourFace inYourFace;
    Direction dir = inYourFace.move(state);
    assertEqual(dir, Direction::Down, "inYourFaceTest1() - go down");
}

void inYourFaceTest3()
{
    GameState state(parseWorld({
        "_ 1 < <",
        "_ _ _ _",
        "_ _ _ _",
        "_ 0 < <",
        "_ _ _ _"
    }));

    InYourFace inYourFace;
    Direction dir = inYourFace.move(state);
    assertEqual(dir, Direction::Up, "inYourFaceTest1() - go up");
}

void simTest1()
{
    GameState state(parseWorld({
        // "_ _ _ _ _ _ > > _ > > > v _ _ _ _",
        // "_ _ _ _ _ _ _ v _ ^ v _ > > v _ _",
        // "* _ _ _ _ _ _ v * ^ < _ _ _ 0 _ _",
        // "* _ * _ * _ _ > > > > > > > > v _",
        // "v < _ _ _ _ _ _ _ _ _ _ _ _ _ v _",
        // "v ^ _ _ _ _ 2 _ _ _ * _ _ _ _ v _",
        // "v _ _ _ _ _ ^ _ _ _ _ _ _ _ _ > v",
        // "v _ _ _ _ * ^ _ _ _ _ _ _ _ _ _ v",
        // "v _ _ * * _ ^ _ * _ 1 < < _ _ _ v",
        // "v _ _ _ _ _ ^ _ _ _ _ _ ^ _ v < <",
        // "v _ _ _ * _ ^ _ _ _ * _ ^ < < _ _",
        // "> > > > > > ^ _ _ _ _ _ _ _ _ _ _"
        "_ _ _ _ _ _ _ _ _ > > > v _ _ _ _",
        "_ _ _ _ _ _ _ v _ ^ _ _ > > v _ _",
        "* _ _ _ _ _ _ v * ^ _ _ _ _ > > 0",
        "* _ * _ * _ 2 > > > > > > > > v _",
        "v < _ _ _ _ ^ _ _ _ _ _ _ _ _ v _",
        "v ^ _ _ _ _ ^ _ _ _ * _ _ _ _ v _",
        "v _ _ _ _ _ ^ _ _ _ _ _ _ _ _ > v",
        "v _ _ _ _ * ^ _ _ 1 < _ _ _ _ _ v",
        "v _ _ * * _ ^ _ * _ ^ < < _ _ _ v",
        "v _ _ _ _ _ ^ _ _ _ _ _ ^ _ v < <",
        "v _ _ _ * _ ^ _ _ _ * _ ^ < < _ _",
        "> > > > > > ^ _ _ _ _ _ _ _ _ _ _"
    }));

    Sim sim(20, 1000);
    Direction dir = sim.move(state);
    assertEqual(dir, Direction::Up, "simTest1() - go up if you want to live");
}

void simTest2()
{
    GameState state(parseWorld({
        "_ * * 0 * _ _ _ _",
        "1 _ * ^ _ * * _ _",
        "^ _ * ^ < _ _ _ _",
        "^ < _ _ ^ v < _ _",
        "> ^ _ _ ^ < ^ _ _",
        "^ _ _ _ _ _ ^ _ _",
        "^ _ _ _ _ _ ^ _ v",
        "^ _ _ _ _ _ * _ v",
        "^ * * * _ _ _ _ v",
        "^ < < < < _ * _ v",
        "* _ * _ ^ < < < <"
    }));

    Sim sim;
    Direction dir = sim.move(state);
    assertEqual(dir, Direction::Right, "simTest2() - go right if you want to live");
}

void simTest3()
{
    GameState state(parseWorld({
        "* _ _ _ _ * _ _ _ * _ * _ _",
        "_ _ _ _ _ _ _ _ _ > v 1 < _",
        "_ _ * _ _ _ > > > ^ v _ ^ _",
        "v < < < _ _ _ _ _ _ v _ ^ *",
        "> v _ _ * _ _ _ _ _ v _ ^ *",
        "* v _ _ _ _ _ _ _ 2 > 0 ^ _",
        "v < _ _ _ _ _ > > ^ _ _ ^ _",
        "> v > > > > > ^ _ _ _ _ ^ *",
        "_ > ^ _ _ _ _ _ _ * _ * ^ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ ^ _",
        "* _ _ _ _ _ * _ _ _ _ _ _ _"
    }));

    Sim sim;
    Direction dir = sim.move(state);
    assertEqual(dir, Direction::Down, "simTest3() - go down if you want to live");
}

void TestSuite::run()
{
    outOfBoundsTests();
    basicGameStateTests();
    mapTests();
    astarTests1();
    astarTests2();
    astarTests3();
    astarTests4();
    astarTests5();
    astarTests6();
    closestFoodTest1();
    closestFoodTest2();
    closestFoodTest3();
    bestFoodTest1();
    bestFoodTest2();
    bestFoodTest3();
    notImmediatelySuicidalTest1();
    notImmediatelySuicidalTest2();
    worldComparisonTest1();
    newStateAfterMovesTest1();
    newStateAfterMovesTest2();
    newStateAfterMovesTest3();
    newStateAfterMovesTest4();
    newStateAfterMovesTest5();
    newStateAfterMovesTest6();
    newStateAfterMovesTest7();
    simulateFuturesTest1();
    simulateFuturesTest2();
    bestMoveTest1();
    directionSetTests();
    arrayDictTest1();
    wideRectangleTest1();
    tallRectangleTest1();
    inYourFaceTest1();
    inYourFaceTest2();
    inYourFaceTest3();
    simTest1();
    simTest2();
    simTest3();
}
