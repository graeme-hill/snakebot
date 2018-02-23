#pragma once

#include <chrono>
#include <functional>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::duration<double> Seconds;

void benchmark(std::string desc, std::function<void()> fn);
