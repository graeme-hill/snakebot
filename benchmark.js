const cpp = require("bindings")("snakebot_native");
const helpers = require("./util/helpers");

cpp.benchmark({
    assertEqual: (actual, expected, message) => {
        console.log("Assertions not supported in benchmarks! Make a test, dummy!");
    },
    parseWorld: (rows) => {
        return helpers.parseWorld(rows);
    }
});

cpp.terminate();