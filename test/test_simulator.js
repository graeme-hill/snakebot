const assert = require("chai").assert;
const movement = require("../util/movement");
const { GameState } = require("../util/game_state");
const helpers = require("../util/helpers");
const simulator = require("../util/simulator");

const tailChaseAlgorithm = {
    meta: {
        name: "tail_chase"
    },
    move: (state) => {
        return movement.chaseTail(state);
    }
};

const eatFoodAlgorithm = {
    meta: {
        name: "eat_food"
    },
    move: (state) => {
        const foodDirection = movement.closestFood(state);
        if (foodDirection) {
            return foodDirection;
        }
        const direction = movement.chaseTail(state);
        return direction;
    }
};

const notAllThatFast = {
    meta: {
        name: "not_all_that_fast"
    },
    move: (state) => {
        // count to a big number to create a minor, synchronous delay
        let count = 0;
        for (let i = 0; i < 10000000; i++) {
            count++;
        }
        return movement.chaseTail(state);
    }
};

describe("simulateFutures() - stop when time runs out", () => {
    const state = new GameState(helpers.parseWorld([
        "_ _ _ _ _ _",
        "_ * _ _ _ _",
        "_ _ _ > v _",
        "_ _ _ _ 0 _",
        "v _ _ _ _ _",
        "> 1 _ _ _ _"
    ]));

    const algorithms = [ notAllThatFast ];
    const possibleFutures = simulator.simulateFutures(
        state, 10, 1000, algorithms);
    const future = possibleFutures[0];

    it("should terminate due to OUT_OF_TIME", () =>
        assert.equal(future.terminationReason, "OUT_OF_TIME"));

    it("should not simulate every possible turn", () =>
        assert.isBelow(future.turnsSimulated, 1000));
});

describe("simulateFutures() - simple tail chase", () => {
    const state = new GameState(helpers.parseWorld([
        "_ _ _ _ _ _",
        "_ * _ _ _ _",
        "_ _ _ > v _",
        "_ _ _ _ 0 _",
        "v _ _ _ _ _",
        "> 1 _ _ _ _"
    ]));
    const algorithms = [ tailChaseAlgorithm ];
    const possibleFutures = simulator.simulateFutures(
        state, 10000000, 100, algorithms);

    it("should have three future", () =>
        assert.lengthOf(possibleFutures, 3));

    const future = possibleFutures[0];

    it("should terminate due to MAX_TURNS", () =>
        assert.equal(future.terminationReason, "MAX_TURNS"));

    it("should simulate 100 turns", () =>
        assert.equal(future.turnsSimulated, 100));

    it("should have no deaths", () =>
        assert.lengthOf(Object.keys(future.obituaries), 0))

    it("should use the right algorithm", () =>
        assert.equal(future.algorithm, tailChaseAlgorithm));

    it ("should repeat same moves after loop starts", () => {
        const expectedMoves = [];
        for (let i = 0; i < 25; i++) {
            expectedMoves.push("left");
            expectedMoves.push("up");
            expectedMoves.push("right");
            expectedMoves.push("down");
        }

        assert.deepEqual(future.moves, expectedMoves);
    });
});

describe("simulateFutures() - eat then tail chase", () => {
    const state = new GameState(helpers.parseWorld([
        "_ _ _ _ _ *",
        "_ _ _ _ _ _",
        "_ _ _ > v _",
        "_ _ _ _ 0 _",
        "v _ _ _ _ _",
        "> 1 _ _ _ _"
    ]));

    const algorithms = [ eatFoodAlgorithm ];
    const possibleFutures = simulator.simulateFutures(
        state, 10000000, 100, algorithms);

    it("should have three future", () =>
        assert.lengthOf(possibleFutures, 3));

    const future = possibleFutures[0];

    it("should terminate due to MAX_TURNS", () =>
        assert.equal(future.terminationReason, "MAX_TURNS"));

    it("should simulate 100 turns", () =>
        assert.equal(future.turnsSimulated, 100));

    it("should have no deaths", () =>
        assert.lengthOf(Object.keys(future.obituaries), 0));

    it("should use the right algorithm", () =>
        assert.equal(future.algorithm, eatFoodAlgorithm));
});

describe("simulateFutures() - fail at eating then tail chase", () => {
    const state = new GameState(helpers.parseWorld([
        "* _ 1 < < _",
        "_ _ _ _ _ _",
        "_ _ _ _ _ _",
        "_ _ _ _ _ _",
        "0 _ _ _ _ _",
        "^ < _ _ _ _"
    ]));
    const algorithms = [ eatFoodAlgorithm ];
    const possibleFutures = simulator.simulateFutures(
        state, 100, 100, algorithms);

    it("should have two futures", () =>
        assert.lengthOf(possibleFutures, 2));

    const future = possibleFutures[0];

    it("should terminate due to MAX_TURNS", () =>
        assert.equal(future.terminationReason, "MAX_TURNS"));

    it("should simulate 100 turns", () =>
        assert.equal(future.turnsSimulated, 100));

    it("should have no deaths", () =>
        assert.lengthOf(Object.keys(future.obituaries), 0));

    it("should have valid food history", () =>
        assert.deepEqual(future.foodsEaten, {
            "1": [2]
        }));

    it("should use the right algorithm", () =>
        assert.equal(future.algorithm, eatFoodAlgorithm));
});

describe("bestMove() - prefer circling over death", () => {
    const state = new GameState(helpers.parseWorld([
        "_ _ _ _ _ v",
        "_ _ _ _ _ v",
        "_ _ _ _ _ v",
        "_ _ _ _ _ 0",
        "_ _ > > 1 _",
        "_ _ _ _ _ *"
    ]));
    const algorithms = [ tailChaseAlgorithm, eatFoodAlgorithm ];
    const possibleFutures = simulator.simulateFutures(
        state, 100000000, 100, algorithms);

    it("should have 8 futures", () =>
        assert.lengthOf(possibleFutures, 8));

    const bestMove = simulator.bestMove(possibleFutures, state);

    it("should choose the non-suicidal future", () => {
        assert.equal(bestMove, "left");
    });
});

describe("bestMove() - prefer easy food over aimless wandering", () => {
    const state = new GameState(helpers.parseWorld([
        "> > 1 _ _ v",
        "v _ _ _ _ v",
        "v _ _ _ _ v",
        "2 _ _ _ _ 0",
        "_ _ _ _ _ _",
        "_ _ _ _ _ *"
    ]));
    const algorithms = [ tailChaseAlgorithm, eatFoodAlgorithm ];
    const possibleFutures = simulator.simulateFutures(
        state, 10, 1000, algorithms);

    it("should have 8 futures", () =>
        assert.lengthOf(possibleFutures, 8));
});
