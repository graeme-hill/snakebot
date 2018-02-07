let helpers = require("./helpers");

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
        for (let key in this._cache) {
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
        this.mySnake = null;

        this._world = world;
        this._gridCache = new GridCache(world.width, world.height);
        this._addOtherSnakes();
        this._addMySnake();
    }

    checkCell(x, y) {
        if (x < 0 || this.width <= x || y < 0 || this.height <= y) {
            return new OutOfBounds();
        }
        let cellContent = this._gridCache.get(x, y);
        return cellContent ? cellContent : new OpenCell();
    }

    _addOtherSnakes() {
        let w = this._world;
        if (w.snakes && helpers.isArray(w.snakes.data)) {
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

module.exports = {
    GameState, OpenCell, OutOfBounds, Snake, SnakePart
};