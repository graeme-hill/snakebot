const express = require('express');
const bodyParser = require('body-parser')
const app = express();

const portFromCommandLine = parseInt(process.argv[2]);
const aiFromCommandLine = process.argv[3];

app.set('port', (process.env.PORT || portFromCommandLine || 5000));

app.use(express.static(__dirname + '/public'));
app.use(bodyParser.json());

const ai = require("./ai/graeme_test");

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
    res.json({
        move: ai.move(req.body)
    });
});

app.listen(app.get('port'), function() {
  console.log('Node app is running on port', app.get('port'));
});
