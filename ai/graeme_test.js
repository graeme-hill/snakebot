let snake = require("../snakelib");

function start(info) {
    // I think there is nothing to do here unless we want to track some state throughout the game???
}

function move(world) {
    let state = new snake.GameState(world);
    let direction = snake.notImmediatelySuicidalMovement(state);
    console.log(direction);
    return direction;
}

function move(world) {
    let state = new snake.GameState(world);
    if (state.hasFood() && snake.mySnakeHasShortestPathToFood(state)) {
        return snake.aStarToNearestFood(state);
    }
    return snake.chaseTailMovement(state);
}

module.exports = { start, move };