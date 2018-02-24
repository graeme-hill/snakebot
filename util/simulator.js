const { Snake, GameState } = require("./game_state");
const movement = require("./movement");

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

function updateObituaries(oldManifest, newManifest, obituaries, turn) {
    for (const oldId in oldManifest) {
        if (!(oldId in newManifest)) {
            obituaries[oldId] = turn;
        }
    }
}

function updateFoodsEaten(oldState, newState, foodsEaten, turn) {
    for (const food of oldState.food) {
        const inThatCellNow = newState.checkCell(food.x, food.y);
        if (inThatCellNow.constructor === Snake) {
            if (!foodsEaten[inThatCellNow.id]) {
                foodsEaten[inThatCellNow.id] = [];
            }
            foodsEaten[inThatCellNow.id].push(turn);
        }
    }
}

function groupName(algorithm, firstMoves) {
    return algorithm.meta.name + "->" + firstMoves.join(",");
}

function* simulate(myAlgo, enemyAlgo, initialState, firstMoves) {
    function makeCurrentResult() {
        return {
            turnsSimulated: turn,
            endState: currentState,
            obituaries,
            foodsEaten,
            algorithm: myAlgo,
            terminationReason,
            moves: simulatedMoves,
            group
        };
    }

    function getMyMove(turn) {
        return firstMoves[turn - 1] || myAlgo.move(currentState);
    }

    const group = groupName(myAlgo, firstMoves);

    // Keep track of how far into the future we have gone
    let turn = 0;

    // Store the result state after present step in simulation
    let currentState = initialState;

    // Keep track of who dies in the future and when
    const obituaries = {};

    // Keep track of who eats in the future and when
    const foodsEaten = {};

    // For logging purposes we want to know why we stopped simulating.
    let terminationReason = null;

    const simulatedMoves = []

    // This loop could go forever if the primary snake stays alive, but since
    // it yields on each iteration the caller can choose how long to keep
    // going. If the caller is the one that decides to stop then it is also
    // responsible for setting a termination reason.
    while (true) {
        turn++;

        const myMove = {
            id: currentState.mySnake.id,
            direction: getMyMove(turn, currentState)
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

        updateObituaries(currentState.snakes, newState.snakes, obituaries, turn);
        updateFoodsEaten(currentState, newState, foodsEaten, turn);

        currentState = newState;

        if (currentState.isLoss()) {
            terminationReason = "LOSS";
            break;
        }

        const currentResult = makeCurrentResult();
        yield currentResult;
    }

    const result = makeCurrentResult();
    return result;
}

function runSimulations(algorithmPairs, initialState, maxMillis, maxTurns, firstMoves) {
    let turn = 0;
    const countdown = new Countdown(maxMillis);

    function coerceTerminationReason(currentReason) {
        if (!currentReason) {
            if (turn >= maxTurns) {
                return "MAX_TURNS";
            } else {
                return "OUT_OF_TIME";
            }
        }
        return currentReason;
    }

    function allSimulationsComplete() {
        return completedSimulations.size === numberedSimulations.length;
    }

    const numberedSimulations = algorithmPairs.map((pair, i) => ({
        index: i,
        sim: simulate(pair.myAlgorithm, pair.enemyAlgorithm, initialState, firstMoves)
    }));

    // Initial results in case zero turns are simulated for some reason
    const results = numberedSimulations.map(s => ({
        turnsSimulated: 0,
        endState: initialState,
        obituaries: {},
        foodsEaten: {},
        algorithm: algorithmPairs[s.index].myAlgorithm,
        terminationReason: null,
        moves: [],
        group: groupName(algorithmPairs[s.index].myAlgorithm, firstMoves)
    }));

    const completedSimulations = new Set();

    while (turn < maxTurns && !countdown.expired() && !allSimulationsComplete()) {
        turn++;
        for (const { index, sim } of numberedSimulations) {
            // don't do anything if this simulation is already complete
            if (completedSimulations.has(index)) {
                continue;
            }

            const { value, done } = sim.next();
            results[index] = value;
            if (done) {
                completedSimulations.add(index);
            }
        }
    }

    for (const result of results) {
        result.terminationReason = coerceTerminationReason(result.terminationReason);
    }

    //console.log("simulated " + turn + " turns");

    return results;
}

function simulateFutures(initialState, maxMillis, maxTurns, algorithms) {
    const algorithmPairs = algorithms.map(a1 => algorithms.map(a2 => ({
        myAlgorithm: a1,
        enemyAlgorithm: a2
    }))).reduce((l, r) => {
        return l.concat(r);
    }, []);

    const possibleFirstMoves = movement.notImmediatelySuicidalMoves(initialState);

    if (possibleFirstMoves.length === 0) {
        // dang... gonna die no matter what. I guess I'll try anyways maybe the simulator
        // find a way to take someone else out with me.
        return runSimulations(algorithmPairs, initialState, maxMillis, maxTurns, []);

    }

    let futures = [];
    for (const firstMove of possibleFirstMoves) {
        futures = futures.concat(runSimulations(algorithmPairs, initialState, maxMillis, maxTurns, [firstMove]));
    }

    return futures;
}

function scoreFuture(future, state) {
    const POINTS_PER_TURN = 100;
    const POINTS_PER_FOOD = 200;

    const myObituary = future.obituaries[state.mySnake.id];
    const myFoodHistory = future.foodsEaten[state.mySnake.id];
    const survivedTurns = myObituary !== undefined ? myObituary : future.turnsSimulated;
    const foodsEaten = myFoodHistory ? myFoodHistory.length : 0;
    const score = survivedTurns * POINTS_PER_TURN
        + foodsEaten * POINTS_PER_FOOD;

    return score;
}

// The best future is the one whose worst case scenario is the least bad
function bestMove(futures, state) {
    const worstCasePerGroup = futures.reduce((worsts, future) => {
        const currentWorst = worsts[future.group];
        const thisScore = scoreFuture(future, state);
        if (currentWorst === undefined || thisScore < currentWorst.score) {
            worsts[future.group] = {
                score: thisScore,
                move: future.moves[0]
            };
        }
        return worsts;
    }, {});

    const best = Object.keys(worstCasePerGroup).reduce((bestScoredMove, scoredMoveKey) => {
        const scoredMove = worstCasePerGroup[scoredMoveKey];
        if (!bestScoredMove || scoredMove.score > bestScoredMove.score) {
            return scoredMove;
        }
        return bestScoredMove;
    }, null);

    //console.log(best);

    return best.move;
}

module.exports = {
    simulateFutures, bestMove
};
