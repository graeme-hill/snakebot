# snakebot

A bot for snake

## Running tests

```
npm install
npm install -g mocha
mocha test
```

## Running a bot

```
node index.js <PORT> <ALGORITHM>
```

`<ALGORITHM>` should be the name of one of the files in the `algorithm/` directory without the extension. For example:

```
node index.js 5000 defensive_hungry
```

## Adding a new bot

Just add a new js file to the `algorithms/` directory and export `move`, `start`, and `meta`.
