#include "timing.hpp"
#include <iostream>

void benchmark(std::string desc, std::function<void()> fn)
{
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::duration<double> Seconds;

    std::cout << desc << "... ";
    auto start = Clock::now();
    fn();
    auto end = Clock::now();
    Seconds duration = end - start;
    auto millis = duration.count() * 1000.0;
    std::cout << millis << std::endl;
}