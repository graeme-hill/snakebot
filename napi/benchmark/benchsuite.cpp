#include "benchsuite.hpp"
#include "../interop.hpp"
#include "../snakelib.hpp"
#include "../algorithms/sim.hpp"
#include "../timing.hpp"

void simulatorOnBusyGrid1()
{
    GameState state(parseWorld({
        "_ _ _ v _ _ _ _ _ _ _ _ _ _ _ _ _ > > v",
        "_ _ 8 v _ 7 < < < < _ _ _ _ _ _ _ _ 9 v",
        "_ _ ^ < _ _ _ _ _ _ * _ _ _ _ _ _ _ ^ <",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ v _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ v _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ 2 _ _ _ _ _",
        "_ _ _ _ > > 0 _ _ _ _ _ _ _ _ _ _ _ * _",
        "_ _ * _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ > v _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ * _ _ _ _ > 4 _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ > > 5 _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ 3 < < _",
        "_ _ _ > > 1 _ _ _ _ _ _ _ _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ * _ _ _ _ _ _ _ _ _ *",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ v _ _ * * * * * * *",
        "_ _ _ _ _ _ _ _ _ _ v _ _ * * * * * * *",
        "_ _ * _ _ _ _ _ _ _ > 6 _ _ _ _ _ _ _ _"
    }));

    Sim sim;

    benchmark("sim - one turn on a big grid w/ a lot of food", [&sim, &state]()
    {
        sim.move(state);
    });
}

void simulatorOnBusyGrid2()
{
    GameState state(parseWorld({
        "_ _ _ v _ _ _ _ _ _ _ _ _ _ _ _ _ > > v",
        "_ _ 8 v _ 7 < < < < _ _ _ _ _ _ _ _ 9 v",
        "_ _ ^ < _ _ _ _ _ _ * _ _ _ _ _ _ _ ^ <",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ v _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ v _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ 2 _ _ _ _ _",
        "_ _ _ _ > > 0 _ _ _ _ _ _ _ _ _ _ _ _ _",
        "_ _ * _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ > v _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ * _ _ _ _ > 4 _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ > > 5 _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ 3 < < _",
        "_ _ _ > > 1 _ _ _ _ _ _ _ _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ *",
        "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ v _ _ _ _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _ _ _ v _ _ _ _ _ _ _ _ _",
        "_ _ * _ _ _ _ _ _ _ > 6 _ _ _ _ _ _ _ _"
    }));

    Sim sim;

    benchmark("sim - one turn on a big grid w/ a couple foods", [&sim, &state]()
    {
        sim.move(state);
    });
}

void BenchSuite::run()
{
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
    simulatorOnBusyGrid1();
    simulatorOnBusyGrid2();
}