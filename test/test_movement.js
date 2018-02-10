const assert = require("chai").assert;
const movement = require("../util/movement");
const { GameState } = require("../util/game_state");
const helpers = require("../util/helpers");

// These are actually mostly testing astar.js but whatever.
describe("chaseTail() - A* pathfinding", () => {
    it("should do simple loop left", () => {
        const state = new GameState(helpers.parseWorld([
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ > v _ _ _",
            "_ _ _ _ 0 _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ v < _ _ _",
            "_ _ _ 1 _ _ _ _"
        ]));
        assert.equal(movement.chaseTail(state), "left");
    });

    it("should do simple loop up", () => {
        const state = new GameState(helpers.parseWorld([
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ v _ _ _",
            "_ _ _ 0 < _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ v _ _ _ _",
            "_ _ _ > 1 _ _ _"
        ]));
        assert.equal(movement.chaseTail(state), "up");
    });

    it("should do simple loop right", () => {
        const state = new GameState(helpers.parseWorld([
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ 0 _ _ _ _",
            "_ _ _ ^ < _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ 1 _ _ _",
            "_ _ _ > ^ _ _ _"
        ]));
        assert.equal(movement.chaseTail(state), "right");
    });

    it("should do simple loop down", () => {
        const state = new GameState(helpers.parseWorld([
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ > 0 _ _ _",
            "_ _ _ ^ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ 1 < _ _ _",
            "_ _ _ _ ^ _ _ _"
        ]));
        assert.equal(movement.chaseTail(state), "down");
    });

    it("should know tail will move out of the way immediately", () => {
        const state = new GameState(helpers.parseWorld([
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ > 0 _ _ _",
            "_ _ _ ^ < _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ 1 < _ _ _",
            "_ _ _ _ ^ _ _ _"
        ]));
        assert.equal(movement.chaseTail(state), "down");
    });

    it("should know tail will move out of the way in time", () => {
        const state = new GameState(helpers.parseWorld([
            "_ _ _ _ _ _ _ _",
            "_ 0 _ _ v < < _",
            "_ ^ < < < _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ 1 < _ _ _",
            "_ _ _ _ ^ _ _ _"
        ]));
        assert.equal(movement.chaseTail(state), "right");
    });

    it("should know tail will NOT move out of the way in time", () => {
        const state = new GameState(helpers.parseWorld([
            "_ _ _ _ _ _ _ _",
            "_ 0 v < _ _ _ _",
            "_ ^ < _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ 1 < _ _ _",
            "_ _ _ _ ^ _ _ _"
        ]));
        assert.equal(movement.chaseTail(state), "up");
    });

    it("should know evemies will move out of the way in time for straight line", () => {
        const state = new GameState(helpers.parseWorld([
            "_ _ _ _ _ _ _ _",
            "_ _ _ 1 2 _ _ _",
            "_ _ _ ^ ^ _ _ _",
            "_ > 0 ^ ^ _ v _",
            "_ ^ _ _ ^ _ v _",
            "_ ^ < < < < < _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _"
        ]));
        assert.equal(movement.chaseTail(state), "right");
    });

    it("should know evemies will NOT move out of the way in time for straight line", () => {
        const state = new GameState(helpers.parseWorld([
            "_ _ _ _ _ _ _ _",
            "_ _ _ 1 2 _ _ _",
            "_ _ _ ^ ^ _ _ _",
            "_ > 0 ^ ^ _ v _",
            "_ ^ _ ^ ^ _ v _",
            "_ ^ < < < < < _",
            "_ _ _ _ _ _ _ _",
            "_ _ _ _ _ _ _ _"
        ]));
        assert.equal(movement.chaseTail(state), "down");
    });
});