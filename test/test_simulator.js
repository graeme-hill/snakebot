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
}

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
}

describe("simulatorFutures()", () => {
    it("should be able to tail chase forever", () => {
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
            state, 100, 100, algorithms);

        assert.lengthOf(possibleFutures, 1, "one future b/c one algorithm supplied");
        
        const future = possibleFutures[0];
        
        assert.equal(future.turnsSimulated, 100, "turns simulated");
        assert.lengthOf(Object.keys(future.obituaries), 0, "no obituaries");
        assert.equal(future.algorithm, tailChaseAlgorithm);
        assert.equal(future.terminationReason, "MAX_TURNS");

        const expectedMoves = [];
        for (let i = 0; i < 25; i++) {
            expectedMoves.push("left");
            expectedMoves.push("up");
            expectedMoves.push("right");
            expectedMoves.push("down");
        }

        assert.deepEqual(future.moves, expectedMoves);
    });

    it("should be able to eat food then tail chase", () => {
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
            state, 100, 100, algorithms);

        assert.lengthOf(possibleFutures, 1, "one future b/c one algorithm supplied");
        
        const future = possibleFutures[0];
        
        assert.equal(future.terminationReason, "MAX_TURNS");
        assert.equal(future.turnsSimulated, 100, "turns simulated");
        assert.lengthOf(Object.keys(future.obituaries), 0, "no obituaries");
        assert.equal(future.algorithm, eatFoodAlgorithm);

        const expectedMoves = [ "right", "up", "up", "up" ];
        for (let i = 0; i < 24; i++) {
            expectedMoves.push("left");
            expectedMoves.push("down");
            expectedMoves.push("right");
            expectedMoves.push("up");
        }

        assert.deepEqual(future.moves, expectedMoves);
    });

    it("should be able to lose food race then tail chase", () => {
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

        assert.lengthOf(possibleFutures, 1, "one future b/c one algorithm supplied");
        
        const future = possibleFutures[0];
        
        assert.equal(future.terminationReason, "MAX_TURNS");
        assert.equal(future.turnsSimulated, 100, "turns simulated");
        assert.lengthOf(Object.keys(future.obituaries), 0, "no obituaries");
        assert.equal(future.algorithm, eatFoodAlgorithm);

        const expectedMoves = [ "up", "up" ];
        for (let i = 0; i < 24; i++) {
            expectedMoves.push("right");
            expectedMoves.push("down");
            expectedMoves.push("left");
            expectedMoves.push("up");
        }
        expectedMoves.push("right");
        expectedMoves.push("down");

        assert.deepEqual(future.moves, expectedMoves);
    });
});