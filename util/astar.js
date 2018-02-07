let helpers = require("./helpers");
let { GameState, OpenCell } = require("./game_state");

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
		let score = fScore[index];
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
	let moves = [];
	while (currentIndex in cameFrom) {
		let nextIndex = cameFrom[currentIndex];
		let direction = helpers.directionBetweenNodes(nextIndex, currentIndex, width);
		moves.unshift(direction);
		currentIndex = nextIndex;
	}
	return moves;
}

function indexIsSafe(index, gameState) {
	let { x, y } = helpers.deconstructCellIndex(index, gameState.width);
	let myTail = gameState.mySnake.tail();
	let isMyTail = myTail.x === x && myTail.y === y;
	return isMyTail || gameState.checkCell(x, y).constructor === OpenCell;
}

// This is for the special case where the AI will turn back on itself in the second move
// of the game when it is only two cells long. That's an illegal move even though the
// tail will no longer be in that position after the move.
function is180(index, neighborIndex, gameState) {
	let head = gameState.mySnake.head();
	let tail = gameState.mySnake.tail();
	let headIndex = helpers.cellIndex(head.x, head.y, gameState.width);
	let tailIndex = helpers.cellIndex(tail.x, tail.y, gameState.width);
	result = index === headIndex && neighborIndex == tailIndex;
	return result;
}

function getNeighbors(index, gameState) {
	let coord = helpers.deconstructCellIndex(index, gameState.width);
	let result = [];

	function addIfSafe(i) {
		if (indexIsSafe(i, gameState) && !is180(index, i, gameState)) {
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
	let startIndex = getIndex(start, gameState);
	let goalIndex = getIndex(goal, gameState);

	// Nodes/cells that have already been evaluated.
	let closedSet = new Set();

	// Nodes/cells that are not yet evaluated but have been discovered. Only the start
	// node is discovered so far.
	let openSet = new Set([ startIndex ]);

	// For each node store the node it can most efficiently be reached from.
	let cameFrom = {};

	// For each node the cost of getting from the start to that node.
	let gScore = {};

	// Already at start so cost of going there is zero.
	gScore[startIndex] = 0;

	// For each node the cost of getting from start to goal passing through that node.
	// Depends on heuristics. Not guaranteed to be precise.
	let fScore = {};

	// fScore for start node is 100% heuristic.
	fScore[startIndex] = heuristicCostEstimate(start, goal);

	while (openSet.size > 0) {
		let currentIndex = lowestFScoreInSet(openSet, fScore);

		if (currentIndex === goalIndex) {
			return reconstructPath(cameFrom, currentIndex, gameState.width);
		}

		openSet.delete(currentIndex);
		closedSet.add(currentIndex);

		let neighbors = getNeighbors(currentIndex, gameState);
		for (let neighborIndex of neighbors) {

			// Don't revisit the same node we've already been to.
			if (closedSet.has(neighborIndex)) {
				continue;
			}

			// Check if we have discovered a new node.
			if (!openSet.has(neighborIndex)) {
				openSet.add(neighborIndex);
			}

			// Distance between neighbors is always 1, hence the +1.
			let tentativeGScore = gScore[currentIndex] + 1;

			// If there's already a better gScore for this node then we're not on the
			// ideal path so bail.
			if (tentativeGScore >= gScore[neighborIndex]) {
				continue;
			}

			// For now this is the best path to this node so record it.
			cameFrom[neighborIndex] = currentIndex;
			gScore[neighborIndex] = tentativeGScore;
			fScore[neighborIndex] = gScore[neighborIndex] + heuristicCostEstimate(
				helpers.deconstructCellIndex(neighborIndex, gameState.width), goal);
		}
	}

	// Finished searching and could not find a path :(
	return null;
}

module.exports = {
	shortestPath
};