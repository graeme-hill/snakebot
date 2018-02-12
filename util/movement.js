const { OpenCell } = require("./game_state");
const astar = require("./astar");
const helpers = require("./helpers");

function notImmediatelySuicidalMoves(gameState) {
    function isOK(x, y) {
        return !helpers.outOfBounds({ x, y }, gameState)
            && gameState.map.turnsUntilVacant({ x, y }) === 0;
    }

    const myHead = gameState.mySnake.head();
    const x = myHead.x;
    const y = myHead.y;
    const moves = [];
    if (isOK(x - 1, y)) {
        moves.push("left");
    }
    if (isOK(x + 1, y)) {
        moves.push("right");
    }
    if (isOK(x, y - 1)) {
        moves.push("up");
    }
    if (isOK(x, y + 1)) {
        moves.push("down");
    }

    return moves;
}

function notImmediatelySuicidal(gameState) {
    return notImmediatelySuicidalMoves(gameState)[0] || "down";
}

function bestFood(gameState) {
    const me = gameState.mySnake;
    let best = null;
    for (const food of gameState.food) {
        const myPath = astar.shortestPath(me.head(), food, gameState);

        // if there is no path then move on
        if (!myPath) {
            continue;
        }

        // if this path isn't even better than another already found then move on
        if (best !== null && myPath.length >= best.length) {
            continue;
        }
        
        const enemyWillWin = gameState.enemies.some(enemy => {
            const enemyGameState = gameState.perspective(enemy);
            const enemyPath = astar.shortestPath(enemy.head(), food, enemyGameState);

            if (!enemyPath) {
                return false;
            }

            if (enemyPath.length === myPath.length) {
                return me.length() < enemy.length();
            }

            return enemyPath.length < myPath.length;
        });

        if (!enemyWillWin) {
            best = myPath;
        }
    }
    
    if (best) {
        return best[0];
    } else {
        return null;
    }
}

function closestFood(gameState) {
    const me = gameState.mySnake;
    let best = null;
    for (const food of gameState.food) {
        const myPath = astar.shortestPath(me.head(), food, gameState);

        // if there is no path then move on
        if (!myPath) {
            continue;
        }

        // if this path isn't even better than another already found then move on
        if (best !== null && myPath.length >= best.length) {
            continue;
        }
        
        best = myPath;
    }
    
    if (best) {
        return best[0];
    } else {
        return null;
    }
}

function chaseTail(gameState) {
    const myHead = gameState.mySnake.head();
    const myTail = gameState.mySnake.tail();
    const path = astar.shortestPath(myHead, myTail, gameState);
    if (path && path.length) {
        return path[0];
    }
    return notImmediatelySuicidal(gameState);
}

module.exports = {
    notImmediatelySuicidal,
    notImmediatelySuicidalMoves,
    chaseTail,
    bestFood,
    closestFood
};