const express = require('express');
const bodyParser = require('body-parser')
const app = express();

const DEFAULT_AI = "graeme_test";
const DEFAULT_PORT = 5000;
const DISPLAY_TIMINGS = true;

const portFromCommandLine = parseInt(process.argv[2]);
const aiFromCommandLine = process.argv[3];

app.set('port', (process.env.PORT || portFromCommandLine || DEFAULT_PORT));

app.use(express.static(__dirname + '/public'));
app.use(bodyParser.json());

const aiName = aiFromCommandLine || DEFAULT_AI;
const ai = require("./ai/" + aiName);

function clock(start) {
    if ( !start ) return process.hrtime();
    var end = process.hrtime(start);
    return Math.round((end[0]*1000) + (end[1]/1000000));
}

app.post('/start', (req, res) => {
    ai.start(req.body);
    res.json({
        color: "#FF0000",
        secondary_color: "#000000",
        head_url: "https://pre00.deviantart.net/1951/th/pre/f/2016/149/3/1/jigglypuff_by_crystal_ribbon-da48ylv.png",
        name: "(>^_^)>",
        taunt: "puff puff",
        head_type: "pixel",
        tail_type: "pixel"
    });
});

app.post('/move', (req, res) => {
    const start = clock();
    res.json({
        move: ai.move(req.body)
    });
    const duration = clock(start);

    if (DISPLAY_TIMINGS) {
        console.log(duration + " millis");
    }
});

app.listen(app.get('port'), function() {
    console.log("snakebot running AI '" + aiName + "' on port " + app.get("port"));
});
