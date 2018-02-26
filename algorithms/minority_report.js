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

function start(id) {
    // I think there is nothing to do here unless we want to track some state throughout the game???
}

function move(world) {
    const state = new GameState(world);
    const algorithms = [ hungry, dumb_dog ];
    const possibleFutures = simulator.simulateFutures(
        state, 100, 50, algorithms);
    const best = simulator.bestMove(possibleFutures, state);

    return best;
}

module.exports = { start, move, meta };
