const express = require('express');
const bodyParser = require('body-parser')
const app = express();

const DEFAULT_ALGORITHM = "cpp_sim";
const DEFAULT_PORT = 5000;
const DISPLAY_TIMINGS = true;

const portFromCommandLine = parseInt(process.argv[2]);
const algorithmFromCommandLine = process.argv[3];

app.set('port', (process.env.PORT || portFromCommandLine || DEFAULT_PORT));

app.use(express.static(__dirname + '/public'));
app.use(bodyParser.json());

function getAlgorithm(key) {
    if (key.substr(0, 4) === "cpp_") {
        // This is a C++ algorithm implementation so load up the binding and
        // make some proxy functions.
        const cppKey = key.substr(4);
        const snakeBotNative = require("bindings")("snakebot_native");
        return {
            meta: snakeBotNative.meta(cppKey),
            move: (world) => snakeBotNative.move(world, cppKey),
            start: (id) => snakeBotNative.start(cppKey, id)
        };
    } else {
        // It's a normal JS algorithm implementation so just require() it.
        return require("./algorithms/" + key);
    }
}

function clock(start) {
    if ( !start ) return process.hrtime();
    var end = process.hrtime(start);
    return Math.round((end[0]*1000) + (end[1]/1000000));
}

const algorithmKey = algorithmFromCommandLine || DEFAULT_ALGORITHM;
const algorithm = getAlgorithm(algorithmKey);

app.post('/start', (req, res) => {
    // start() is optional
    if (algorithm.start) {
        algorithm.start(req.body.game_id.toString());
    }
    res.json(algorithm.meta);
});

app.post('/move', (req, res) => {
    const start = clock();

    // docs say it is a string but actually it's an int, so just convert it to
    // a string so it will work either way
    req.body.id = req.body.id.toString();
    res.json({
        move: algorithm.move(req.body)
    });
    const duration = clock(start);

    if (DISPLAY_TIMINGS) {
        console.log(duration + " millis");
    }
});

app.get('/', (req, res) => {
    res.json({
        activeAlgorithm: algorithm.meta
    });
});

app.listen(app.get('port'), function() {
    console.log("snakebot running AI '" + algorithmKey + "' on port " + app.get("port"));
});
