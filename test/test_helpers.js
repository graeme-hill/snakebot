const assert = require("chai").assert;
const helpers = require("../util/helpers");

function requireSnake(id, snakes) {
    if (!snakes.find) {
        snakes = snakes.data;
    }
    const snake = snakes.find(s => s.id === id.toString());
    assert.exists(snake, "snake " + id + " should exist");
    return snake;
}

describe("cellIndex()", () => {
    it("works with zero", () =>
        assert.equal(helpers.cellIndex(0, 0, 8), 0));

    it("works at end of row", () =>
        assert.equal(helpers.cellIndex(7, 2, 8), 23));

    it("works in middle of row", () =>
        assert.equal(helpers.cellIndex(4, 3, 8), 28));
});

describe("deconstructCellIndex()", () => {
    it("works with zero", () => {
        const actual = helpers.deconstructCellIndex(0, 100);
        assert.deepEqual(actual, { x: 0, y: 0 });
    });

    it("works at end of row", () => {
        const actual = helpers.deconstructCellIndex(199, 100);
        assert.deepEqual(actual, { x: 99, y: 1 });
    });

    it("works in middle of row", () => {
        const actual = helpers.deconstructCellIndex(250, 100);
        assert.deepEqual(actual, { x: 50, y: 2 });
    });
});

describe("parseWorld() - a couple snakeroos", () => {
    const world = helpers.parseWorld([
        "_ _ _ _ _ _ _ _",
        "_ _ _ v _ _ _ _",
        "_ _ _ > v _ _ _",
        "_ _ _ _ 0 _ _ _",
        "_ * _ _ _ _ _ _",
        "_ _ _ _ _ _ _ _",
        "_ _ _ v < < < _",
        "_ _ _ 1 _ _ _ _"
    ]);

    const youFromSnakes = world.snakes.data.find(s => s.id === world.you.id);

    it("should have two snakes", () =>
        assert.equal(world.snakes.data.length, 2));
    it("should identify 'you'", () =>
        assert.deepEqual(world.you, youFromSnakes));

    const snake0 = requireSnake(0, world.snakes);
    const snake1 = requireSnake(1, world.snakes);

    it("should have valid snake0", () => {
        assert.deepEqual(snake0, {
            body: {
                data: [
                    { object: "point", x: 4, y: 3 },
                    { object: "point", x: 4, y: 2 },
                    { object: "point", x: 3, y: 2 },
                    { object: "point", x: 3, y: 1 }
                ]
            },
            length: 4,
            id: "0",
            health: 100
        });
    });

    it("should have valid snake1", () => {
        assert.deepEqual(snake1, {
            body: {
                data: [
                    { object: "point", x: 3, y: 7 },
                    { object: "point", x: 3, y: 6 },
                    { object: "point", x: 4, y: 6 },
                    { object: "point", x: 5, y: 6 },
                    { object: "point", x: 6, y: 6 }
                ]
            },
            length: 5,
            id: "1",
            health: 100
        });
    });

    it("should have correct world width", () =>
        assert.equal(world.width, 8));
    it("should have correct world height", () =>
        assert.equal(world.height, 8));

    it("should have correct food", () => {
        assert.deepEqual(world.food, {
            data: [
                { object: "point", x: 1, y: 4 }
            ]
        });
    });
});