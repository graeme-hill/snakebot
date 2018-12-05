#include "timing.hpp"
#include <iostream>

void benchmark(std::string desc, std::function<void()> fn)
{
    std::cout << desc << "... ";
    auto start = Clock::now();
    fn();
    auto end = Clock::now();
    Seconds duration = end - start;
    auto millis = duration.count() * 1000.0;
    std::cout << millis << std::endl;
}
