const { GameState } = require("../util/game_state");
const movement = require("../util/movement");

function logDecision(direction, state, tag) {
    const coord = state.mySnake.head().x + "," + state.mySnake.head().y;
    console.log(direction + " (" + coord + ") - " + tag);
}

const meta = {
    color: "#FF0000",
    secondary_color: "#000000",
    head_url: "https://pre00.deviantart.net/1951/th/pre/f/2016/149/3/1/jigglypuff_by_crystal_ribbon-da48ylv.png",
    name: "Defensive Hungry",
    taunt: "yum yum",
    head_type: "pixel",
    tail_type: "pixel"
};

function start(id) {
    // I think there is nothing to do here unless we want to track some state throughout the game???
}

function move(inState) {
    const state = inState.constructor === GameState ? inState : new GameState(inState);

    //state.map.printVacateGrid();

    // Check if there is any food worth going after.
    const foodDirection = movement.bestFood(state);
    if (foodDirection) {
        //logDecision(foodDirection, state, "food");
        return foodDirection;
    }

    // Probably not gonna get the food so just defensively loop around.
    const direction = movement.chaseTail(state);
    //logDecision(direction, state, "chase");
    return direction;
}

module.exports = { start, move, meta };
