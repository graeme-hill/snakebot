const helpers = require("./helpers");

function cloneWorld(world) {
    return JSON.parse(JSON.stringify(world));
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

    printOccupied() {
        for (const key in this._cache) {
            console.log(key);
            console.log(helpers.deconstructCellIndex(key, this._width));
        }
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
        this._gridCache = new GridCache(world.width, world.height);
        this._addOtherSnakes();
        this._addMySnake();
        this._perspectiveCopies = {};

        this.map = new MapInfo(this);
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
        const snake = new Snake(snakeData.id);
        this.snakes[snake.id] = snake;
        if (snake.id !== this._world.you.id) {
            this.enemies.push(snake);
        }
        for (const partData of snakeData.body.data) {
            this._addSnakePart(snake, partData);
        }
        return snake;
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
        try {
            return this._cells[y][x].vacated || 0;
        } catch (e) {
            debugger;
        }
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