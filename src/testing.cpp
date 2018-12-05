#include "testing.hpp"
#include <iostream>

std::vector<Assertion> AssertionAggregator::_assertions;

void AssertionAggregator::addAssertion(Assertion assertion)
{
    std::string clrPrefix = assertion.actual == assertion.expected
        ? "\033[1;32m"
        : "\033[1;31m";
    std::string undoFormatting = "\033[0m";
    std::cout << clrPrefix << assertion.actual << " == " << assertion.expected
        << " --> " << assertion.message << undoFormatting << std::endl;
    AssertionAggregator::_assertions.push_back(assertion);
}

void AssertionAggregator::summarize()
{
    uint32_t passed = 0;
    uint32_t failed = 0;

    for (auto &assertion : AssertionAggregator::_assertions)
    {
        if (assertion.expected == assertion.actual)
            passed++;
        else
            failed++;
    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << passed << " passed" << std::endl;
    std::cout << failed << " failed" << std::endl;
    if (failed == 0)
    {
        std::cout << ":)" << std::endl;
    }
    else
    {
        std::cout << ":(" << std::endl;
    }
}

void assertEqual(std::string actual, std::string expected, std::string message)
{
    AssertionAggregator::addAssertion({ actual, expected, message });
}

void assertEqual(uint32_t actual, uint32_t expected, std::string message)
{
    std::stringstream ass;
    std::stringstream ess;
    ass << actual;
    ess << expected;
	assertEqual(ass.str(), ess.str(), message);
}

void assertEqual(Direction actual, Direction expected, std::string message)
{
    std::string actualStr = directionToString(actual);
    std::string expectedStr = directionToString(expected);
    assertEqual(actualStr, expectedStr, message);
}

void assertEqual(
    TerminationReason actual, TerminationReason expected, std::string message)
{
    std::string actualStr = terminationReasonToString(actual);
    std::string expectedStr = terminationReasonToString(expected);
    assertEqual(actualStr, expectedStr, message);
}

void assertEqual(Point p1, Point p2, std::string message)
{
    std::stringstream p1ss;
    std::stringstream p2ss;
    p1ss << "(" << p1.x << "," << p1.y << ")";
    p2ss << "(" << p2.x << "," << p2.y << ")";
    assertEqual(p1ss.str(), p2ss.str(), message);
}

void assertEqual(World &w1, World &w2, std::string messagePart)
{
    assertEqual(w2.width, w1.width, messagePart + " - width");
    assertEqual(w2.height, w1.height, messagePart + " - height");
    assertEqual(w2.food.size(), w1.food.size(), messagePart + " - food count");

    if (w2.food.size() == w1.food.size())
    {
        for (size_t i = 0; i < w2.food.size(); i++)
        {
            std::stringstream msg;
            msg << messagePart << " - food " << i;
            assertEqual(w2.food[i], w1.food[i], msg.str());
        }
    }

    assertEqual(
        w2.snakes.size(), w1.snakes.size(), messagePart + " - snake count");

    if (w2.snakes.size() == w1.snakes.size())
    {
        for (size_t i = 0; i < w2.snakes.size(); i++)
        {
            std::stringstream msg;
            msg << messagePart << " - snake " << i;
            assertEqual(w2.snakes[i], w1.snakes[i], msg.str());
        }

        if (w2.snakes.size() > 0)
        {
            assertEqual(w2.you, w1.you, messagePart + " - you");
        }
    }
}

void assertEqual(Snake &s1, Snake &s2, std::string messagePart)
{
    assertEqual(s2.id, s1.id, messagePart + " - id");
    assertEqual(s2.length(), s1.length(), messagePart + " - length");

    if (s2.length() == s1.length())
    {
        for (size_t i = 0; i < s2.parts.size(); i++)
        {
            std::stringstream msg;
            msg << messagePart << " - part " << i;
            assertEqual(s2.parts[i], s1.parts[i], msg.str());
        }
    }
}

void assertEqual(Future &f1, Future &f2, std::string msgPart)
{
    assertEqual(
        f2.obituaries.size(), f1.obituaries.size(), msgPart + " - obit size");

    if (f2.obituaries.size() == f1.obituaries.size())
    {
        for (auto pair : f2.obituaries)
        {
            auto it = f1.obituaries.find(pair.first);
            assertTrue(
                it != f2.obituaries.end(),
                msgPart + " - key '" + pair.first + "' is present");
            if (it != f2.obituaries.end())
            {
                assertEqual(
                    pair.second, it->second, msgPart + " - same values");
            }
        }
    }

    assertEqual(
        f2.foodsEaten.size(),
        f1.foodsEaten.size(),
        msgPart + " - foods eaten size");

    if (f2.foodsEaten.size() == f1.foodsEaten.size())
    {
        for (auto pair : f2.foodsEaten)
        {
            auto it = f1.foodsEaten.find(pair.first);
            assertTrue(
                it != f2.foodsEaten.end(),
                msgPart + " - key '" + pair.first + "' is present");
            if (it != f2.foodsEaten.end())
            {
                std::vector<uint32_t> turns1 = it->second;
                std::vector<uint32_t> turns2 = pair.second;
                assertEqual(
                    turns2.size(),
                    turns1.size(),
                    msgPart + " - same food count");
                if (turns2.size() == turns1.size())
                {
                    for (size_t i = 0; i < turns2.size(); i++)
                    {
                        std::stringstream ss;
                        assertEqual(
                            turns2[i], turns1[i], msgPart + " - same turn");
                    }
                }
            }
        }
    }

    assertEqual(
        f2.source.pair.myAlgorithm->meta().name,
        f1.source.pair.myAlgorithm->meta().name,
        msgPart + " - algorithm");
    assertTrue(
        f2.terminationReason == f1.terminationReason,
        msgPart + " - termination reason");
    assertEqual(f2.turns, f1.turns, msgPart + " - turns");
    assertEqual(f2.move, f1.move, msgPart + " - move");
}

void assertEqual(
    std::vector<Future> &f1, std::vector<Future> &f2, std::string m)
{
    assertEqual(f2.size(), f1.size(), m + " - future count");

    if (f2.size() == f1.size())
    {
        for (size_t i = 0; i < f2.size(); i++)
        {
            std::stringstream msg;
            msg << m << " - future " << i;
            assertEqual(f2[i], f1[i], msg.str());
        }
    }
}

void assertTrue(bool actual, std::string message)
{
    std::string actualStr = actual ? "true" : "false";
    assertEqual(actualStr, "true", message);
}

struct ArrowOffset
{
    char ch;
    int x;
    int y;
};

bool isNumber(char ch)
{
    // Works because ascii values for 0-9 are in consecutive ascending order.
    return ch >= '0' && ch <= '9';
}

bool isFood(char ch)
{
    return ch == '*';
}

World parseWorld(std::vector<std::string> strings)
{
    std::vector<std::string> symbols;
    for (std::string &row : strings)
    {
        std::string symbolRow;
        for (char ch : row)
        {
            if (ch == 'V')
                ch = 'v';

            if (ch != ' ')
                symbolRow.push_back(ch);
        }
        symbols.push_back(symbolRow);
    }

    World w;
    w.width = symbols.at(0).size();
    w.height = symbols.size();

    w.you = "0";

    std::array<ArrowOffset, 4> offsets;
    offsets[0] = {'<', 1, 0};
    offsets[1] = {'>', -1, 0};
    offsets[2] = {'^', 0, 1};
    offsets[3] = {'v', 0, -1};

    std::function<MaybePoint(Point)> findNextPart =
        [offsets, w, &symbols](Point p)
        {
            for (ArrowOffset offset : offsets)
            {
                Point pNext = { p.x + offset.x, p.y + offset.y };
                if (!outOfBounds(pNext, w.width, w.height))
                {
                    if (offset.ch == symbols[pNext.y][pNext.x])
                    {
                        return MaybePoint::just(pNext);
                    }
                }
            }
            return MaybePoint::none();
        };

    std::function<std::vector<Point>(Point)> makeSnakeParts =
        [findNextPart](Point start)
        {
            std::vector<Point> result { start };
            MaybePoint next = findNextPart(start);
            while (next.hasValue)
            {
                result.push_back(next.value);
                next = findNextPart(next.value);
            }
            return result;
        };

    for (auto &row : symbols)
    {
        if (row.size() != w.width)
        {
            std::cerr << "Bad map. All rows should be the same length.";
            throw "Bad map. All rows should be the same length.";
        }
    }

    for (uint32_t rowIndex = 0; rowIndex < w.height; rowIndex++)
    {
        std::string &row = symbols.at(rowIndex);
        for (uint32_t colIndex = 0; colIndex < row.size(); colIndex++)
        {
            char ch = row.at(colIndex);
            if (isNumber(ch))
            {
                std::string snakeNumber(1, ch);
                std::vector<Point> parts = makeSnakeParts(
                    { colIndex, rowIndex });
                Snake snake { snakeNumber, 100, parts, false };
                w.snakes.push_back(snake);
            }
            else if (isFood(ch))
            {
                w.food.push_back({ colIndex, rowIndex });
            }
        }
    }

    return w;
}
