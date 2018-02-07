const { GameState } = require("../util/game_state");
const movement = require("../util/movement");

function start(info) {
    // I think there is nothing to do here unless we want to track some state throughout the game???
}

function move(world) {
    const state = new GameState(world);

    // Check if there is any food worth going after.
    const food = movement.bestFoodOption(state);
    if (food) {
        const foodDirection = movement.getFood(food, state);
        if (foodDirection) {
            return foodDirection;
        }
    }

    // Probably not gonna get the food so just defensively loop around.
    const direction = movement.chaseTail(state);
    return direction;
}

module.exports = { start, move };