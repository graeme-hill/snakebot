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

module.exports = { start, move };