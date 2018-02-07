let { OpenCell } = require("./game_state");
let astar = require("./astar");

function notImmediatelySuicidal(gameState) {
    function isOK(x, y) {
        return gameState.checkCell(x, y).constructor === OpenCell;
    }

    let myHead = gameState.mySnake.head();
    let x = myHead.x;
    let y = myHead.y;
    if (isOK(x - 1, y)) {
        return "left";
    }
    if (isOK(x + 1, y)) {
        return "right";
    }
    if (isOK(x, y - 1)) {
        return "up";
    }
    return "down";
}

// This should probably give the food with shortest path from my snake, but for now
// just return the first one.
function bestFoodOption(gameState) {
    return gameState.food[0];
}

function getFood(foodCoord, gameState) {
    let myHead = gameState.mySnake.head();
    let path = astar.shortestPath(myHead, foodCoord, gameState);
    return path ? path[0] : null;
}

function chaseTail(gameState) {
    let myHead = gameState.mySnake.head();
    let myTail = gameState.mySnake.tail();
    let path = astar.shortestPath(myHead, myTail, gameState);
    if (path && path.length) {
        return path[0];
    }
    return notImmediatelySuicidal(gameState);
}

module.exports = {
    notImmediatelySuicidal,
    chaseTail,
    bestFoodOption,
    getFood
};