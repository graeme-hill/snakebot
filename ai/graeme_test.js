const { GameState } = require("../util/game_state");
const movement = require("../util/movement");

function logDecision(direction, state, tag) {
    const coord = state.mySnake.head().x + "," + state.mySnake.head().y;
    console.log(direction + " (" + coord + ") - " + tag);
}

function start(info) {
    // I think there is nothing to do here unless we want to track some state throughout the game???
}

function move(world) {
    const state = new GameState(world);

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

module.exports = { start, move };