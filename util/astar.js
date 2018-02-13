const helpers = require("./helpers");
const { GameState, OpenCell } = require("./game_state");

const VERY_HIGH_COST = 1000;

function getIndex(coord, gameState) {
	return helpers.cellIndex(coord.x, coord.y, gameState.width);
}

// Just calculate the distance if there were no barriers.
function heuristicCostEstimate(start, goal) {
	return Math.abs(start.x - goal.x) + Math.abs(start.y - goal.y);
}

function lowestFScoreInSet(set, fScore) {
	let lowest = null;
	let lowestIndex = null;
	set.forEach((index) => {
		const score = fScore[index];
		if (score !== undefined && score !== null) {
			if (lowest === null || score < lowest) {
				lowest = score;
				lowestIndex = index;
			}
		}
	});
	return lowestIndex;
}

function reconstructPath(cameFrom, currentIndex, width) {
	const moves = [];
	while (currentIndex in cameFrom) {
		const nextIndex = cameFrom[currentIndex];
		const direction = helpers.directionBetweenNodes(nextIndex, currentIndex, width);
		moves.unshift(direction);
		currentIndex = nextIndex;
	}
	return moves;
}

function indexIsSafe(index, turn, gameState) {
	const coord = helpers.deconstructCellIndex(index, gameState.width);
	if (helpers.outOfBounds(coord, gameState)) {
		return false;
	}
	const turnsUntilCouldBeVacant = gameState.map.turnsUntilVacant(coord);
	return turnsUntilCouldBeVacant < turn;
	// const myTail = gameState.mySnake.tail();
	// const isMyTail = myTail.x === x && myTail.y === y;
	// return isMyTail || gameState.checkCell(x, y).constructor === OpenCell;
}

function isAdjacent(aIndex, bIndex, gameState) {
	const a = helpers.deconstructCellIndex(aIndex, gameState.width);
	const b = helpers.deconstructCellIndex(bIndex, gameState.width);

	const horizontallyAdjacent = Math.abs(a.x - b.x) === 1 && a.y === b.y;
	const verticallyAdjacent = Math.abs(a.y - b.y) === 1 && a.x === b.x;

	return horizontallyAdjacent || verticallyAdjacent;
}

// This is for the special case where the AI will turn back on itself in the second move
// of the game when it is only two cells long. That's an illegal move even though the
// tail will no longer be in that position after the move.
function is180(index, neighborIndex, gameState) {
	const head = gameState.mySnake.parts[0];
	const neck = gameState.mySnake.parts[1];

	if (!neck) {
		// just in case length of 1 is allowed
		return false;
	}

	const headIndex = helpers.cellIndex(head.x, head.y, gameState.width);
	const neckIndex = helpers.cellIndex(neck.x, neck.y, gameState.width);
	return index === headIndex && neighborIndex == neckIndex;
}

function isCloseToHead(index, snake, gameState) {
	const headIndex = helpers.cellIndex(snake.head().x, snake.head().y, gameState.width);
	return isAdjacent(index, headIndex, gameState);
}

function isCloseToEqualOrBiggerSnakeHead(index, gameState) {
	for (const otherSnake of gameState.enemies) {
		const otherSnakeIsTooBigToEat = gameState.mySnake.length() <= otherSnake.length();
		if (otherSnakeIsTooBigToEat && isCloseToHead(index, otherSnake, gameState)) {
			return true;
		}
	}
	return false;
}

function getNeighbors(index, turn, gameState) {
	const coord = helpers.deconstructCellIndex(index, gameState.width);
	const result = [];

	function addIfSafe(i) {
		if (indexIsSafe(i, turn, gameState) && !is180(index, i, gameState) && isAdjacent(index, i, gameState)) {
			result.push(i);
		}
	}

	addIfSafe(index + 1); // right
	addIfSafe(index - 1); // left
	addIfSafe(index + gameState.width); // down
	addIfSafe(index - gameState.width); // up

	return result;
}

// Partially stolen from the pseudo-code in the wikipedia page for A*:
// https://en.wikipedia.org/wiki/A*_search_algorithm#Pseudocode
function shortestPath(start, goal, gameState) {

	const startIndex = getIndex(start, gameState);
	const goalIndex = getIndex(goal, gameState);

	// Allow some special functionality on first move to avoid being eaten.
	let isFirstMove = true;

	// Nodes/cells that have already been evaluated.
	const closedSet = new Set();

	// Nodes/cells that are not yet evaluated but have been discovered. Only the start
	// node is discovered so far.
	const openSet = new Set([ startIndex ]);

	// For each node store the node it can most efficiently be reached from.
	const cameFrom = {};

	// For each node the cost of getting from the start to that node.
	const gScore = {};

	// Like gScore but just straight up turn count to get to each cell. gScore could
	// be exagerrated to discourage certain risky paths.
	const turns = {};

	// Already at start so cost of going there is zero.
	gScore[startIndex] = 0;
	
	// Start at turn 1 because it makes more sense in my head.
	turns[startIndex] = 1;

	// For each node the cost of getting from start to goal passing through that node.
	// Depends on heuristics. Not guaranteed to be precise.
	const fScore = {};

	// fScore for start node is 100% heuristic.
	fScore[startIndex] = heuristicCostEstimate(start, goal);

	while (openSet.size > 0) {
		const currentIndex = lowestFScoreInSet(openSet, fScore);

		if (currentIndex === goalIndex) {
			return reconstructPath(cameFrom, currentIndex, gameState.width);
		}

		openSet.delete(currentIndex);
		closedSet.add(currentIndex);

		const neighbors = getNeighbors(currentIndex, turns[currentIndex], gameState);
		for (const neighborIndex of neighbors) {
			// Don't revisit the same node we've already been to.
			if (closedSet.has(neighborIndex)) {
				continue;
			}

			// Check if we have discovered a new node.
			if (!openSet.has(neighborIndex)) {
				openSet.add(neighborIndex);
			}

			// On immediate move avoid getting eaten.
			let dontGetEatenModifier = 0;
			if (isFirstMove) {
				const couldGetEaten = isCloseToEqualOrBiggerSnakeHead(neighborIndex, gameState);
				if (couldGetEaten) {
					dontGetEatenModifier = VERY_HIGH_COST;
				}
			}

			// Distance between neighbors is always 1, hence the +1. Also if there is a chance that
			// you could get eaten in this sport then consider it much higher cost.
			const tentativeGScore = gScore[currentIndex] + 1 + dontGetEatenModifier;

			// If there's already a better gScore for this node then we're not on the
			// ideal path so bail.
			if (tentativeGScore >= gScore[neighborIndex]) {
				continue;
			}

			// For now this is the best path to this node so record it.
			cameFrom[neighborIndex] = currentIndex;
			gScore[neighborIndex] = tentativeGScore;
			turns[neighborIndex] = turns[currentIndex] + 1;
			fScore[neighborIndex] = gScore[neighborIndex] + heuristicCostEstimate(
				helpers.deconstructCellIndex(neighborIndex, gameState.width), goal);
		}

		isFirstMove = false;
	}

	// Finished searching and could not find a path :(
	return null;
}

module.exports = {
	shortestPath
};