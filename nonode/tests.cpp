#include "testing.hpp"
#include "simulator.hpp"
#include "test/testsuite.hpp"

int main()
{
    SimThread::startAll();
    TestSuite::run();
    AssertionAggregator::summarize();
    SimThread::stopAll();
    return 0;
}
