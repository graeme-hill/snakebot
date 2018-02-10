class Countdown {
    constructor(maxMillis) {
        this._maxMillis = maxMillis;
        this._start = process.hrtime();
    }

    expired() {
        const elapsed = process.hrtime(this._start);
        const millis = Math.round((elapsed[0]*1000) + (elapsed[1]/1000000));
        return millis >= this._maxMillis;
    }
}

function ranOutOfTime() {
    // TODO ...
}

// Get a list of snakes that disappeared 
function updateObituaries(oldManifest, newManifest, obituaries, turn) {
    for (const oldId in oldManifest) {
        if (!(oldId in newManifest)) {
            obituaries[oldId] = turn;
        }
    }
}

function simulate(myAlgo, enemyAlgo, initialState, countdown, maxTurns) {
    //console.log("START OF SIM");
    //initialState._gridCache.prettyPrint();
    //initialState.map.printVacateGrid();

    // Keep track of how far into the future we have gone
    let turn = 0;

    // Store the result state after present step in simulation
    let currentState = initialState;

    // Keep track of who dies in the future and when
    const obituaries = {};

    // For logging purposes we want to know why we stopped simulating.
    let terminationReason = "OUT_OF_TIME";

    const simulatedMoves = []

    while (!countdown.expired()) {
        if (turn >= maxTurns) {
            terminationReason = "MAX_TURNS";
            break;
        }

        turn++;

        const myMove = {
            id: currentState.mySnake.id,
            direction: myAlgo.move(currentState)
        };
        const enemyMoves = currentState.enemies.map(enemy => {
            return {
                id: enemy.id,
                direction: enemyAlgo.move(currentState.perspective(enemy))
            };
        });

        simulatedMoves.push(myMove.direction);

        const allMoves = [myMove].concat(enemyMoves);

        const newState = currentState.newStateAfterMoves(allMoves);

        //console.log("AFTER TURN " + turn);
        //newState._gridCache.prettyPrint();
        //newState.map.printVacateGrid();

        updateObituaries(currentState.snakes, newState.snakes, obituaries, turn);

        currentState = newState;

        if (currentState.isLoss()) {
            terminationReason = "LOSS";
            break;
        }

        if (currentState.isWin()) {
            terminationReason = "WIN";
            break;
        }
    }

    const result = {
        turnsSimulated: turn,
        endState: currentState,
        obituaries,
        algorithm: myAlgo,
        terminationReason,
        moves: simulatedMoves
    };

    //console.log("sim " + myAlgo.meta.name + "/" + enemyAlgo.meta.name + " " + turn + " turns starting w/ " + result.firstMove + " ending with " + terminationReason);
    //console.log(simulatedMoves);

    return result;
}

function simulateFutures(initialState, maxMillis, maxTurns, algorithms) {
    const algorithmPairs = algorithms.map(a1 => algorithms.map(a2 => {
        return {
            myAlgorithm: a1,
            enemyAlgorithm: a2
        };
    })).reduce((l, r) => {
        return l.concat(r);
    }, []);

    // Equally divide simulation time between all the timelines.
    const timePerPair = maxMillis / algorithmPairs.length;

    const futures = algorithmPairs.map(p =>
        simulate(p.myAlgorithm, p.enemyAlgorithm, initialState, new Countdown(timePerPair), maxTurns))

    return futures;
}

module.exports = {
    simulateFutures
};