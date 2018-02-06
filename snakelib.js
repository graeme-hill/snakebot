function isArray(x) {
    return x !== null && x !== undefined && x.constructor === Array;
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
        console.log("set " + x + ", " + y + " to " + this._cellIndex(x, y));
        this._cache[this._cellIndex(x, y)] = value;
    }

    get(x, y) {
        return this._cache[this._cellIndex(x, y)];
    }

    printOccupied() {
        for (let key in this._cache) {
            console.log(key);
            console.log(this._deconstructCellIndex(key));
        }
    }

    _deconstructCellIndex(index) {
        return {
            x: index % this._width,
            y: Math.floor(index / this._width)
        };
    }
    
    _cellIndex(x, y) {
        return this._width * y + x;
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
        this._world = world;
        this._gridCache = new GridCache(world.width, world.height);
        this.snakes = {};
        this.mySnake = null;
        this._addOtherSnakes();
        this._addMySnake();
    }

    checkCell(x, y) {
        if (x < 0 || this._world.width <= x || y < 0 || this._world.height <= y) {
            return new OutOfBounds();
        }
        let cellContent = this._gridCache.get(x, y);
        return cellContent ? cellContent : new OpenCell();
    }

    _addOtherSnakes() {
        let w = this._world;
        if (w.snakes && isArray(w.snakes.data)) {
            for (let snake of w.snakes.data) {
                this._addSnake(snake);
            }
        }
    }

    _addMySnake() {
        let w = this._world;
        if (w && w.you) {
            this.mySnake = this._addSnake(w.you);
        }
    }

    _addSnake(snakeData) {
        let snake = new Snake(snakeData.id);
        this.snakes[snake.id] = snake;
        for (let partData of snakeData.body.data) {
            this._addSnakePart(snake, partData);
        }
        return snake;
    }

    _addSnakePart(snake, partData) {
        let part = new SnakePart(partData.x, partData.y);
        snake.addPart(part);
        this._gridCache.set(part.x, part.y, snake);
    }
}

function notImmediatelySuicidalMovement(gameState) {
    function isOK(x, y) {
        gameState._gridCache.printOccupied();
        let result = gameState.checkCell(x, y).constructor;
        return result === OpenCell;
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

function chaseTailMovement(gameState) {
    // todo ...
}

module.exports = {
    GameState, notImmediatelySuicidalMovement, chaseTailMovement
};