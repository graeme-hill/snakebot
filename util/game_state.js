const helpers = require("./helpers");

function cloneWorld(world) {
    return JSON.parse(JSON.stringify(world));
}

function coordAfterMove(x, y, direction) {
    switch (direction) {
        case "up": return { x, y: y - 1 };
        case "down": return { x, y: y + 1 };
        case "left": return { x: x - 1, y };
        default: return { x: x + 1, y };
    }
}

/* Simulates a two dimensional dictionary. Really it's just a one dimension dictionary
 * where each cell in the 2D grid gets a unique id. Example IDs for a 3x3 grid:
 *    0 1 2
 *    3 4 5
 *    6 7 8
 */
class GridCache {
    constructor(width, height) {
        this._width = width;
        this._height = height;
        this._cache = {};
    }

    set(x, y, value) {
        this._cache[helpers.cellIndex(x, y, this._width)] = value;
    }

    get(x, y) {
        return this._cache[helpers.cellIndex(x, y, this._width)];
    }

    clear(x, y) {
        delete this._cache[helpers.cellIndex(x, y, this._width)];
    }

    printOccupied() {
        for (const key in this._cache) {
            console.log(key);
            console.log(helpers.deconstructCellIndex(key, this._width));
        }
    }

    prettyPrint() {
        console.log("--- grid cache ---");
        for (let row = 0; row < this._height; row++) {
            let rowStr = "";
            for (let col = 0; col < this._width; col++) {
                const val = this.get(col, row);
                const x = val && val.constructor === Snake ? "x" : "_";
                rowStr += x + " ";
            }
            console.log(rowStr);
        }
        console.log("------------------");
    }
}

class Snake {
    constructor(id) {
        this.id = id;
        this.parts = []; // they get added later by addPart
    }

    addPart(part) {
        this.parts.push(part);
        part.snake = this;
    }

    tail() {
        return this.parts[this.parts.length - 1];
    }

    head() {
        return this.parts[0];
    }

    length() {
        return this.parts.length;
    }
}

class SnakePart {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.snake = null; // gets set later when Snake.addPart is called
    }
}

class OutOfBounds { }

class OpenCell { }

/* The data from the world api object but stored in a way that makes it easier/faster
 * to access data spacially.
 */
class GameState {
    constructor(world) {
        this.width = world.width;
        this.height = world.height;
        this.snakes = {};
        this.enemies = [];
        this.mySnake = null;
        this.food = world.food.data;

        this._world = world;
        this._buildCaches();
    }

    _buildCaches() {
        this._gridCache = new GridCache(this.width, this.height);
        this._addOtherSnakes();
        this._addMySnake();
        this._perspectiveCopies = {};
        this.map = new MapInfo(this);
    }

    // Makes a duplicate of this game state with the given moves applied. Useful
    // if you are trying to simulate future game state.
    newStateAfterMoves(moves) {
        const newWorld = cloneWorld(this._world);
        const newState = new GameState(newWorld);

        // // Find moves that result in death
        // const notDeadMoves = [];

        // for (const { id, direction } of moves) {
        //     const snake = newState.snakes[id];
        //     const head = snake.head();
        //     const destination = coordAfterMove(head.x, head.y, direction);
        //     const turnsUntilDestinationVacant = this.map.turnsUntilVacant(destination);
        //     if (turnsUntilDestinationVacant > 0) {
        //         newState.
        //     } else {
        //         notDeadMoves.push({ id, direction });
        //     }
        // }

        // Shift snake parts
        for (const { id, direction } of moves) {
            const snake = newState.snakes[id];
            const head = snake.head();
            const destination = coordAfterMove(head.x, head.y, direction);
            const turnsUntilDestinationVacant = this.map.turnsUntilVacant(destination);
            const outOfBounds = helpers.outOfBounds(destination, this);
            if (turnsUntilDestinationVacant > 0 || outOfBounds) {
                // It will take more than one turn for this cell to become vacant, therefore
                // you're colliding and you are dead. We know that it's not a head on collision
                // because the head moves every turn. Will need a separate condition to catch
                // that (below).
                newState._removeSnake(id);
            } else {
                // Add new head pos.
                const newPart = new SnakePart(destination.x, destination.y);
                newPart.snake = snake;
                snake.parts.unshift(newPart);
                newState._gridCache.set(newPart.x, newPart.y, snake);
                const worldSnake = newState._world.snakes.data.find(s => s.id === id);
                worldSnake.body.data.unshift({
                    object: "point",
                    x: newPart.x,
                    y: newPart.y
                });

                // Chop off end of tail
                const [removed] = snake.parts.splice(snake.parts.length - 1);
                worldSnake.body.data.splice(worldSnake.body.data.length - 1, 1);

                // If this part was in the cache and the cell is now vacant then update the
                // cache. Note that the cell will not be vacant if this is one of the first
                // moves of the game where the snake is all bunched up in one cell.
                const existingCached = newState._gridCache.get(removed.x, removed.y);
                const newEndOfTail = snake.parts[snake.parts.length - 1];
                const endOfTailMoved = newEndOfTail.x !== removed.x || newEndOfTail.y !== removed.y;
                if (existingCached && existingCached.id === id && endOfTailMoved) {
                    newState._gridCache.clear(removed.x, removed.y);
                }

                // If just ate food then double up on new tail position so that it stays in
                // place on next movement and grows the snake. Also remove that food from
                // game state.
                const justAteSomeFood = newState.food.some(f =>
                    f.x === destination.x && f.y === destination.y);
                if (justAteSomeFood) {
                    const bunched = new SnakePart(newEndOfTail.x, newEndOfTail.y);
                    bunched.snake = snake;
                    worldSnake.body.data.push({
                        object: "point",
                        x: bunched.x,
                        y: bunched.y
                    });
                    worldSnake.length = worldSnake.body.data.length;
                    snake.parts.push(bunched);

                    newState._removeFood(destination.x, destination.y);
                }

                newState.map.incrementalUpdate(snake, newPart, justAteSomeFood);

                // Make sure the world 'you' snake is up to date
                if (!newState.mySnake) {
                    newState._world.you = null;
                } else if (worldSnake.id === newState.mySnake.id) {
                    newState._world.you = worldSnake;
                }
            }
        }

        // Check for head on collisions
        for (const id in newState.snakes) {
            const thisSnake = newState.snakes[id];
            if (!thisSnake) {
                continue;
            }

            const myHeadPos = thisSnake.head();
            for (const otherId in newState.snakes) {
                if (otherId === id) {
                    continue;
                }
                const otherSnake = newState.snakes[otherId];
                if (!otherSnake) {
                    continue;
                }

                const otherHeadPos = otherSnake.head();
                if (myHeadPos.x === otherHeadPos.x && myHeadPos.y === otherHeadPos.y) {
                    const myLength = newState.snakes[id].length();
                    const otherLength = newState.snakes[otherId].length();
                    if (myLength <= otherLength) {
                        newState._removeSnake(id);
                    }
                    if (otherLength <= myLength) {
                        newState._removeSnake(otherId);
                    }
                }
            }
        }

        // Remove eaten food
        newState.food = newState.food.filter(p => newState.checkCell(p.x, p.y).constructor === OpenCell);

        return newState;
    }

    isLoss() {
        return !this.mySnake;
    }

    isWin() {
        return !this.enemies.length;
    }

    // Makes a copy of this game state but from the perspective of the given enemy. This means
    // that in code like "gameState.mySnake" MY actually refers to the enemy snake whose perspective
    // we are simulating. Useful if you want to predict other pleyers' movements.
    perspective(enemy) {
        if (enemy.id in this._perspectiveCopies) {
            return this._perspectiveCopies[enemy.id];
        }

        const newWorld = cloneWorld(this._world);
        newWorld.you = newWorld.snakes.data.find(s => s.id === enemy.id);
        let newState = new GameState(newWorld);
        this._perspectiveCopies[enemy.id] = newState;
        return newState;
    }

    hasFood() {
        return !!this.food.length;
    }

    checkCell(x, y) {
        if (x < 0 || this.width <= x || y < 0 || this.height <= y) {
            return new OutOfBounds();
        }
        const cellContent = this._gridCache.get(x, y);
        return cellContent ? cellContent : new OpenCell();
    }

    _removeFood(x, y) {
        const index = this.food.findIndex(f => f.x === x && f.y === y);
        if (index >= 0) {
            this.food.splice(index, 1);
        }

        const worldIndex = this._world.food.data.findIndex(f => f.x === x && f.y === y);
        if (worldIndex >= 0) {
            this._world.food.data.splice(worldIndex, 1);
        }
    }

    _removeSnake(id) {
        const parts = this.snakes[id].parts;

        // If I am the one that died make sure mySnake is null
        if (this.mySnake && this.mySnake.id === id) {
            this.mySnake = null;
            this._world.you = null;
        }

        // If an enemy died then remove from enemies
        const enemyIndex = this.enemies.findIndex(e => e.id === id);
        if (enemyIndex >= 0) {
            this.enemies.splice(enemyIndex, 1);
        }

        // Also update the world object because that will be used in future duplicates
        const worldIndex = this._world.snakes.data.findIndex(s => s.id === id);
        if (worldIndex >= 0) {
            this._world.snakes.data.splice(worldIndex, 1);
        }

        delete this.snakes[id];
        this.map.removeSnake(parts);

        // With a snake removed there are several different conditions for updating the
        // caches. eg: head-on-tail, head-on-head, head-on-self, two-heads-on-same-tail, etc
        // so instead of trying to handle all of those it is easier to just rebuild them
        // completely.
        this._buildCaches();
    }

    _addOtherSnakes() {
        const w = this._world;
        if (w.snakes && helpers.isArray(w.snakes.data)) {
            for (const snake of w.snakes.data) {
                this._addSnake(snake);
            }
        }
    }

    _addMySnake() {
        const w = this._world;
        if (w && w.you) {
            this.mySnake = this._addSnake(w.you);
        }
    }

    _addSnake(snakeData) {
        try {
            const snake = new Snake(snakeData.id);
            this.snakes[snake.id] = snake;
            if (this._world.you && snake.id !== this._world.you.id) {
                this.enemies.push(snake);
            }
            for (const partData of snakeData.body.data) {
                this._addSnakePart(snake, partData);
            }
            return snake;
        } catch (e) {
            debugger;
            throw e;
        }
    }

    _addSnakePart(snake, partData) {
        const part = new SnakePart(partData.x, partData.y);
        snake.addPart(part);
        this._gridCache.set(part.x, part.y, snake);
    }
}

class CellOccupancy {
    constructor() {
        this.vacated = null;
        this.entered = null;
    }

    enter(turn) {
        if (turn !== null && turn !== undefined) {
            if (!this.entered || turn < this.entered) {
                this.entered = turn;
            }
        }
    }

    vacate(turn) {
        if (turn !== null && turn !== undefined) {
            if (!this.vacated || turn > this.vacated) {
                this.vacated = turn;
            }
        }    
    }

    decrementVacate() {
        if (this.vacated) {
            this.vacated--;
        }
    }

    resetVacate() {
        this.vacated = 0;
    }
}

class MapInfo {
    constructor(gameState) {
        this._gameState = gameState;
        this._cells = [];

        this._initCells();
        this._updateVacateTurns();
        // this._updateEnemyEnterTurns();
    }

    turnsUntilVacant({ x, y }) {
        if (helpers.outOfBounds({ x, y }, this._gameState)) {
            return 0;
        }
        return this._cells[y][x].vacated || 0;
    }

    printVacateGrid() {
        console.log("--- vacate grid ---");
        for (let row = 0; row < this._gameState.height; row++) {
            let rowStr = "";
            for (let column = 0; column < this._gameState.width; column++) {
                rowStr += (this._cells[row][column].vacated || 0);
            }
            console.log(rowStr);
        }

        console.log("-------------------");
    }

    removeSnake(parts) {
        for (const { x, y } of parts) {
            this._cells[y][x].resetVacate();
        }
    }

    incrementalUpdate(snake, newHeadPos, ateFood) {
        this._cells[newHeadPos.y][newHeadPos.x].vacate(snake.parts.length - 1);
        if (!ateFood) {
            for (let i = 1; i < snake.parts.length; i++) {
                const { x, y } = snake.parts[i];
                this._cells[y][x].decrementVacate();
            }
        }
    }

    _initCells() {
        for (let row = 0; row < this._gameState.height; row++) {
            this._cells.push([]);
            for (let column = 0; column < this._gameState.width; column++) {
                this._cells[row].push(new CellOccupancy());
            }
        }
    }

    _updateVacateTurns() {
        for (const id in this._gameState.snakes) {
            const snake = this._gameState.snakes[id];
            this._updateVacateTurnsForSnake(snake);
        }
    }

    _updateVacateTurnsForSnake(snake) {
        for (let i = 0; i < snake.parts.length; i++) {
            const { x, y } = snake.parts[i];
            const vacated = snake.parts.length - i - 1;
            this._cells[y][x].vacate(vacated);
        }
    }

    // _updateEnemyEnterTurns() {
    //     for (const snake of this._gameState.enemies) {
    //         this._updateEnterTurnsForSnake(snake, snake.head(), 1);
    //     }
    // }

    // _updateEnterTurnsForSnake(snake) {
    //     const visited = new Set();
    //     const pending = [coord];

    //     function outOfBounds({ x, y }) {
    //         return x < 0 || x >= this._gameState.width || y < 0 || y >= this._gameState.height;
    //     }

    //     function hasBeenVisited({ x, y }) {
    //         return visited.has(helpers.cellIndex(x, y, this._gameState.width));
    //     }

    //     function isVisitableNeighbor({ x, y }) {
    //         return !outOfBounds(x, y) && !hasBeenVisited(x, y)
    //     }

    //     function neighbors({ x, y }) {
    //         const left = { x: x - 1, y };
    //         const right = { x: x + 1, y };
    //         const up = { x, y: y - 1 };
    //         const down = { x, y: y + 1 };

    //         const allNeighbors = [ left, right, up, down ];
    //         const visitable = allNeighbors.filter(isVisitableNeighbor);

    //         return visitable;
    //     }

    //     // can re-enter current head position after as many turns as their are parts in the snake
    //     this._cells[y][x]

    //     while (true) {
    //         const currentCoord = pending.pop();
    //         if (!currentCoord) {
    //             break;
    //         }

    //         const neighborCoords = neighbors(currentCoord);
    //         for (const { x, y } of neighborCoords) {
    //             this._cells[y][x].enter(turn);
    //             visited.add(helpers.cellIndex(x, y, this._gameState.width));
    //             pending.push({ x, y });
    //         }
    //     }
    // }
}


module.exports = {
    GameState, OpenCell, OutOfBounds, Snake, SnakePart
};