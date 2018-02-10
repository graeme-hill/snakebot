const { GameState } = require("../util/game_state");
const simulator = require("../util/simulator");
const hungry = require("./hungry");
const dumb_dog = require("./dumb_dog");

function logDecision(direction, state, tag) {
    const coord = state.mySnake.head().x + "," + state.mySnake.head().y;
    console.log(direction + " (" + coord + ") - " + tag);
}

const meta = {
    color: "#FFFF00",
    secondary_color: "#000000",
    head_url: "https://pre00.deviantart.net/1951/th/pre/f/2016/149/3/1/jigglypuff_by_crystal_ribbon-da48ylv.png",
    name: "Minority Report",
    taunt: "I don't know what I'm doing",
    head_type: "pixel",
    tail_type: "pixel"
};

function start(info) {
    // I think there is nothing to do here unless we want to track some state throughout the game???
}

function move(world) {
    const state = new GameState(world);
    const algorithms = [ hungry ]; // hungry
    const possibleFutures = simulator.simulateFutures(
        tate, 100, 100, algorithms);
    const best = bestMove(possibleFutures, state);

    return best;
}

function scoreFuture(future, state) {
    const POINTS_PER_TURN = 100;

    const myObituary = future.obituaries[state.mySnake.id];
    const survivedTurns = myObituary !== undefined ? myObituary : future.turnsSimulated;
    const score = survivedTurns * POINTS_PER_TURN;

    return score;
}

// The best future is the one whose worst case scenario is the least bad
function bestMove(futures, state) {
    const worstCasePerAlgorithm = futures.reduce((worsts, future) => {
        const currentWorst = worsts[future.algorithm.meta.name];
        const thisScore = scoreFuture(future, state);
        if (currentWorst === undefined || thisScore < currentWorst.score) {
            worsts[future.algorithm.meta.name] = {
                score: thisScore,
                move: future.moves[0]
            };
        }
        return worsts;
    }, {});

    const best = Object.keys(worstCasePerAlgorithm).reduce((bestScoredMove, scoredMoveKey) => {
        const scoredMove = worstCasePerAlgorithm[scoredMoveKey];
        if (!bestScoredMove || scoredMove.score < bestScoredMove.score) {
            return scoredMove;
        }
        return bestScoredMove;
    }, null);

    console.log(best);

    return best.move;
}

module.exports = { start, move, meta };