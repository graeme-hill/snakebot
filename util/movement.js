const { OpenCell } = require("./game_state");
const astar = require("./astar");

function notImmediatelySuicidal(gameState) {
    function isOK(x, y) {
        return gameState.checkCell(x, y).constructor === OpenCell;
    }

    const myHead = gameState.mySnake.head();
    const x = myHead.x;
    const y = myHead.y;
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
    chaseTail,
    bestFood,
    closestFood
};