#pragma once

#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include "snakelib.hpp"
#include "simulator.hpp"

struct Assertion
{
    std::string actual;
    std::string expected;
    std::string message;
};

class AssertionAggregator
{
public:
    static void addAssertion(Assertion assertion);
    static void summarize();

private:
    static std::vector<Assertion> _assertions;
};

void assertEqual(std::string actual, std::string expected, std::string message);

void assertEqual(uint32_t actual, uint32_t expected, std::string message);

void assertEqual(Direction actual, Direction expected, std::string message);

void assertEqual(TerminationReason actual, TerminationReason expected, std::string message);

void assertEqual(World &w1, World &w2, std::string messagePart);

void assertEqual(Snake &s1, Snake &s2, std::string messagePart);

void assertEqual(Point p1, Point p2, std::string message);

void assertEqual(Future &f1, Future &f2, std::string msgPart);

void assertEqual(std::vector<Future> &f1, std::vector<Future> &f2, std::string m);

void assertTrue(bool actual, std::string message);

World parseWorld(std::vector<std::string> stringy);
