const cpp = require("bindings")("snakebot_native");
const assert = require("chai").assert;
const movement = require("../util/movement");
const { GameState } = require("../util/game_state");
const helpers = require("../util/helpers");

describe("move()", () => {
    const world = helpers.parseWorld([
        "_ _ _ _ _ _ _ _",
        "_ * _ _ _ _ _ _",
        "_ _ _ > v _ _ _",
        "_ _ _ _ 0 _ _ _",
        "_ _ _ _ _ _ _ *",
        "_ _ _ _ _ _ _ _",
        "_ _ _ v < _ _ _",
        "_ _ _ 1 _ _ _ _"
    ]);

    it("should return a direction", () =>
        assert.include(["left","right","up","down"], cpp.move(world, "test")));
});

describe("meta()", () => {
    it("should have metadata", () => {
        assert.deepEqual(cpp.meta("test"), {
            color: "#000000",
            secondary_color: "#FFFFFF",
            head_url: "http://www.edm2.com/images/thumb/1/13/C%2B%2B.png/250px-C%2B%2B.png",
            name: "C++ test",
            taunt: "Hi",
            head_type: "pixel",
            tail_type: "pixel"
        });
    });
});

describe("C++ tests", () => {
    cpp.test({
        assertEqual: (actual, expected, message) => {
            it(message, () => assert.equal(actual, expected));
        },
        parseWorld: (rows) => {
            return helpers.parseWorld(rows);
        }
    });
});