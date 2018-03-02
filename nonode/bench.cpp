#include "benchmark/benchsuite.hpp"
#include "simulator.hpp"

int main()
{
    SimThread::startAll();
    BenchSuite::run();
    SimThread::stopAll();
}
