let { GameState } = require("../util/game_state");
let movement = require("../util/movement");

// function justGoToTheTopLeftForSomeReason(gameState) {
//     let myHead = gameState.mySnake.head();
//     let path = astar.shortestPath(myHead, { x: 0, y: 0 }, gameState);
//     console.log(path);
//     return path[0];
// }

function start(info) {
    // I think there is nothing to do here unless we want to track some state throughout the game???
}

function move(world) {
    let state = new GameState(world);
    if (state.hasFood()) {
        let foodCoord = movement.bestFoodOption(state);
        let foodDirection = movement.getFood(foodCoord, state);
        if (foodDirection) {
            return foodDirection;
        }
    }
    let direction = movement.chaseTail(state);
    return direction;
}

// function move(world) {
//     let state = new snake.GameState(world);
//     if (state.hasFood() && snake.mySnakeHasShortestPathToFood(state)) {
//         return snake.aStarToNearestFood(state);
//     }
//     return snake.chaseTailMovement(state);
// }

module.exports = { start, move };