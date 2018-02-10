/* Example input:
 *
 * [
 *     "_ _ _ _ _ _ _ _",
 *     "_ _ _ v _ _ _ _",
 *     "_ _ _ > v _ _ _",
 *     "_ _ _ _ 0 _ _ _",
 *     "_ * _ _ _ _ _ _",
 *     "_ _ _ _ _ _ _ _",
 *     "_ _ _ v < < < _",
 *     "_ _ _ 1 _ _ _ _"
 * ]
 *
 * Each part of the snake is represented by '<', 'v', '>', or '^' pointing
 * in the direction of the next part belonging to the same snake. The head
 * is a number between 0 and 9 that uniquely identifies that snake. This
 * weirdness is necessary to make it possible to identify each part's owner
 * with only a single character.
 *
 * Whitespace is ignored so the spaces are not required.
 * 
 * Supports up to ten snakes (same as game limit) so that they will fit in
 * one char each. Snake 0 is "my" snake (which is actually called "you" in the
 * world json object that the game sends).
 */
function parseWorld(stringy) {
    // These offsets show the direction you would look when searching from head
    // part, so they are actually the opposite of the directions the arrows
    // point.
    const offsets = [
        { char: "<", x: 1, y: 0 },
        { char: ">", x: -1, y: 0 },
        { char: "^", x: 0, y: 1 },
        { char: "v", x: 0, y: -1 }
    ];

    function isNumber(char) {
        return "0123456789".indexOf(char) >= 0;
    }

    function isFood(char) {
        return char === "*";
    }

    function makePart(row, col) {
        return {
            object: "point",
            x: col,
            y: row
        };
    }

    function testSymbol(row, col, expected) {
        if (row < 0 || col < 0 || row >= world.height || col >= world.width) {
            return false;
        }
        return symbols[row][col] === expected;
    }

    function findNextPart(row, col) {
        for (const offset of offsets) {
            const nextRow = row + offset.y;
            const nextCol = col + offset.x;
            if (testSymbol(nextRow, nextCol, offset.char)) {
                return { nextRow, nextCol };
            }
        }
        return null;
    }

    function makeSnakeParts(row, col, parts) {
        parts = parts || [];
        parts = parts.concat([makePart(row, col)]);
        const next = findNextPart(row, col);
        if (next) {
            parts = makeSnakeParts(next.nextRow, next.nextCol, parts);
        }
        return parts;
    }

    function makeSnake(number, parts) {
        return {
            body: {
                data: parts
            },
            health: 100, // TODO: do not hard code
            length: parts.length, // TODO: consider possible bunching on first turns
            id: number.toString(), // normally this is a guid but any string should work the same
        };
    }

    function makeFood(row, col) {
        return {
            object: "point",
            x: col,
            y: row
        };
    }

    const world = {
        food: {
            data: []
        },
        snakes: {
            data: []
        }
    };

    const symbols = stringy.map(s => s.replace(/\s/g, "").toLowerCase());
    world.width = symbols[0].length;
    world.height = symbols.length;

    const allRowsAreSameLength = symbols.every(row => world.width === row.length);
    if (!allRowsAreSameLength) {
        throw "Bad map. All rows should be the same length.";
    }

    for (let rowIndex = 0; rowIndex < symbols.length; rowIndex++) {
        const row = symbols[rowIndex];
        for (let colIndex = 0; colIndex < row.length; colIndex++) {
            const char = row[colIndex];
            if (isNumber(char)) {
                const snakeNumber = parseInt(char);
                const parts = makeSnakeParts(rowIndex, colIndex);
                const snake = makeSnake(snakeNumber, parts);
                world.snakes.data.push(snake);
                if (snakeNumber === 0) {
                    world.you = snake;
                }
            } else if (isFood(char)) {
                world.food.data.push(makeFood(rowIndex, colIndex));
            }
        }
    }

    return world;
}

function isArray(x) {
    return x !== null && x !== undefined && x.constructor === Array;
}

function deconstructCellIndex(index, width) {
    return {
        x: index % width,
        y: Math.floor(index / width)
    };
}

function cellIndex(x, y, width) {
    return width * y + x;
}

function directionBetweenNodes(fromIndex, toIndex, width) {
    const fromCoord = deconstructCellIndex(fromIndex, width);
    const toCoord = deconstructCellIndex(toIndex, width);

    // Find first sensible direction assuming that they are adjacent and not diagonal.
    if (fromCoord.x < toCoord.x) {
        return "right";
    }
    if (toCoord.x < fromCoord.x) {
        return "left";
    }
    if (fromCoord.y < toCoord.y) {
        return "down";
    }
    return "up";
}

function outOfBounds({ x, y }, gameState) {
    return x < 0 || x >= gameState.width || y < 0 || y >= gameState.height;
}

module.exports = {
    isArray,
    deconstructCellIndex,
    cellIndex,
    directionBetweenNodes,
    outOfBounds,
    parseWorld
};