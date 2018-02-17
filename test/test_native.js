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
        assert.include(["left","right","up","down"], cpp.move(world, "dog")));
});

describe("meta()", () => {
    it("should have metadata", () => {
        assert.deepEqual(cpp.meta("dog"), {
            color: "#000088",
            secondary_color: "#FFFFFF",
            head_url: "http://residentevents.com/media/reviews/photos/original/84/ad/76/hungry20hungry20hippos-78-1440787989.jpg",
            name: "Dog",
            taunt: "Woof",
            head_type: "pixel",
            tail_type: "pixel"
        });
    });
});

describe("C++ tests", function() {
    after(function() {
        cpp.terminate();
    });

    cpp.test({
        assertEqual: (actual, expected, message) => {
            it(message, () => assert.equal(actual, expected));
        },
        parseWorld: (rows) => {
            return helpers.parseWorld(rows);
        }
    });
});