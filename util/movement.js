let { OpenCell } = require("./game_state");

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

function chaseTail(gameState) {
    // todo ...
}

module.exports = {
    notImmediatelySuicidal,
    chaseTail
};