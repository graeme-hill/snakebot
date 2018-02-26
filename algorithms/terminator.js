const { GameState } = require("../util/game_state");
const movement = require("../util/movement");

const meta = {
    color: "#ff6347", //tomato
    secondary_color: "#d8bfd8", //thistle
    head_url: "http://webiconspng.com/wp-content/uploads/2017/09/Terminator-PNG-Image-10416.png",
    name: "Terminator",
    taunt: "Two Weeks, Two Weeks, two weeeekss",
    head_type: "pixel",
    tail_type: "pixel"
};

function start(id) {
    // I think there is nothing to do here unless we want to track some state throughout the game???
}

function move(inState) {
    const state = inState.constructor === GameState ? inState : new GameState(inState);

    const killTunnelDirection = movement.closestKillTunnelTarget(state);

    if (killTunnelDirection) {
        console.log("KILL TUNNEL, going: " +killTunnelDirection);
        return killTunnelDirection;
    }

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
