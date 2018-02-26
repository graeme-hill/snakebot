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

function identifyEnemyTunnelTarget(enemies, gameState) {
    const myHead = gameState.mySnake.head();
    function checkCell(previousCell, x, y) {
        if(previousCell.x === x && previousCell.y === y) {
            return 1;
        }
        var cellState = gameState.checkCell(x, y);
        if(cellState.constructor === OpenCell) {
            return 0;
        } else {
            return 1;
        }
    }

    const cellDirectionMappings = [ 
        (cell) => { //move north
            console.log("map north");
            return { x: cell.x, y: cell.y - 1 };
        }, 
        (cell) => { //move south
            console.log("map south");
            return { x: cell.x, y: cell.y + 1 };
        }, 
        (cell) => { //move west
            console.log("map wwest");
            return { x: cell.x - 1, y: cell.y };
        }, 
        (cell) => { //move east
            console.log("map east");
            return { x: cell.x + 1, y: cell.y };
        }
    ];

    const sumFxn = (x, sum) => { 
        sum += x;
        return sum;
    };

    var closestKillPath = null;
    enemies.forEach((enemy, enemyIndex) => {
        const head = enemy.head();

        let currentCell = { x: head.x, y: head.y };

        const directionsOccupied = [0,0,0,0]; //north south west east
        directionsOccupied[0] = checkCell({}, currentCell.x, currentCell.y-1);
        directionsOccupied[1] = checkCell({},  currentCell.x, currentCell.y+1);
        directionsOccupied[2] = checkCell({},  currentCell.x-1, currentCell.y);
        directionsOccupied[3] = checkCell({},  currentCell.x+1, currentCell.y);
        
       // console.log("enemy: " + enemyIndex + " : " + directionsOccupied);
        const cellPath = [];
        let bailOutCounter = 0;
        while (directionsOccupied.reduce(sumFxn, 0) === 3 && bailOutCounter < gameState.width) { //should check height too
            console.log("loop " + directionsOccupied);
            let indexOfCellWhoIsEmpty;
            directionsOccupied.some((occupied, index) => { 
                if(occupied === 0) {
                    indexOfCellWhoIsEmpty = index;
                    return true;
                }
            });
            
            let previousCell = { x: currentCell.x, y: currentCell.y };
            currentCell = cellDirectionMappings[indexOfCellWhoIsEmpty](currentCell);            
            cellPath.push({x: currentCell.x, y: currentCell.y });
            directionsOccupied[0] = checkCell(previousCell, currentCell.x, currentCell.y-1);
            directionsOccupied[1] = checkCell(previousCell, currentCell.x, currentCell.y+1);
            directionsOccupied[2] = checkCell(previousCell, currentCell.x-1, currentCell.y);
            directionsOccupied[3] = checkCell(previousCell, currentCell.x+1, currentCell.y);
            bailOutCounter++;
        }

        const targetCell = cellPath.length > 1 ? cellPath[cellPath.length - 1] : null;
        if(targetCell) {
            console.log(cellPath);
            console.log("TargetCell found: ");
            console.log(targetCell);
            var myPath = astar.shortestPath(myHead, targetCell, gameState);
            if (!myPath) {
                return;
            }

            if(myPath.length > cellPath.length) {//if it takes me longer to get there then for them to escape, ignore it
                return
            }

            // if this path isn't even better than another already found then move on
            if (closestKillPath !== null && myPath.length >= closestKillPath.length) {
                return;
            }

            closestKillPath = myPath;
        }
    });
    
    if (closestKillPath) {
        console.log("KILL PATH FOUND: " + closestKillPath);
        return closestKillPath[0];
    } else {
        return null;
    }
}

function closestKillTunnelTarget(gameState) {
    const me = gameState.mySnake;
    const enemies = gameState.enemies;
    const candidatePath = identifyEnemyTunnelTarget(enemies, gameState);
    return candidatePath;
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
    closestFood,
    closestKillTunnelTarget
};