const { GameState } = require("../util/game_state");
const movement = require("../util/movement");

const meta = {
    color: "#FF00FF",
    secondary_color: "#000000",
    head_url: "https://pre00.deviantart.net/1951/th/pre/f/2016/149/3/1/jigglypuff_by_crystal_ribbon-da48ylv.png",
    name: "Dumb Dog",
    taunt: "woof",
    head_type: "pixel",
    tail_type: "pixel"
};

function start(id) {
    // I think there is nothing to do here unless we want to track some state throughout the game???
}

function move(inState) {
    const state = inState.constructor === GameState ? inState : new GameState(inState);
    return movement.chaseTail(state);
}

module.exports = { start, move, meta };
