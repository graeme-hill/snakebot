#pragma once

#include <chrono>
#include <functional>

void benchmark(std::string desc, std::function<void()> fn);