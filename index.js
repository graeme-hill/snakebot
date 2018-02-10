const express = require('express');
const bodyParser = require('body-parser')
const app = express();

const DEFAULT_AI = "defensive_hungry";
const DEFAULT_PORT = 5000;
const DISPLAY_TIMINGS = true;

const portFromCommandLine = parseInt(process.argv[2]);
const aiFromCommandLine = process.argv[3];

app.set('port', (process.env.PORT || portFromCommandLine || DEFAULT_PORT));

app.use(express.static(__dirname + '/public'));
app.use(bodyParser.json());

const algorithmName = aiFromCommandLine || DEFAULT_AI;
const algorithm = require("./algorithms/" + algorithmName);

function clock(start) {
    if ( !start ) return process.hrtime();
    var end = process.hrtime(start);
    return Math.round((end[0]*1000) + (end[1]/1000000));
}

app.post('/start', (req, res) => {
    algorithm.start(req.body);
    res.json(algorithm.meta);
});

app.post('/move', (req, res) => {
    const start = clock();
    res.json({
        move: algorithm.move(req.body)
    });
    const duration = clock(start);

    if (DISPLAY_TIMINGS) {
        console.log(duration + " millis");
    }
});

app.listen(app.get('port'), function() {
    console.log("snakebot running AI '" + algorithmName + "' on port " + app.get("port"));
});
