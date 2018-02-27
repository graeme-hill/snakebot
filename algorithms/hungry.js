const { GameState } = require("../util/game_state");
const movement = require("../util/movement");

const meta = {
    color: "#00FFFF",
    secondary_color: "#000000",
    head_url: "https://pre00.deviantart.net/1951/th/pre/f/2016/149/3/1/jigglypuff_by_crystal_ribbon-da48ylv.png",
    name: "Hungry",
    taunt: "yum yum",
    head_type: "pixel",
    tail_type: "pixel"
};

function start(id) {
    // I think there is nothing to do here unless we want to track some state throughout the game???
}

function move(inState) {
    const state = inState.constructor === GameState ? inState : new GameState(inState);

    // Check if there is any food worth going after.
    const foodDirection = movement.closestFood(state);
    if (foodDirection) {
        //logDecision(foodDirection, state, "food");
        return foodDirection;
    }

    // There is no accessible food so loop around.
    const direction = movement.chaseTail(state);
    return direction;
}

module.exports = { start, move, meta };
